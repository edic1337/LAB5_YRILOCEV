#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "search.h"
#include "index.h"
#include "../btree/btree.h"
#include "../avl/avl.h"
#include "../rbtree/rbtree.h"
#define MAX_TOKENS 128

Vector* intersectTwoPostings(Vector* v1, Vector* v2) {
    Vector* result = createVector(sizeof(values_from_keys));
    int i = 0; // указатель для 1 вектора
    int j = 0; // указатель для 2 вектора

    while (i < v1 -> size && j < v2 -> size) {
        values_from_keys* val1 = (values_from_keys*)getVectorItem(v1, i);
        values_from_keys* val2 = (values_from_keys*)getVectorItem(v2, j);
        if (val1 -> doc_id == val2 -> doc_id) {
            appendVectorItem(result, val1);
            i++;
            j++;
        }
        else if (val1 -> doc_id < val2 -> doc_id) {
            i++; // знаем что векторы отсортированы, просто смещаем указатель на большее число
        }
        else {
            j++;
        }
    }
    return result;
}

Vector* intersectPostings(Vector** lists, int n) {
    if (n == 0) {
        return createVector(sizeof(values_from_keys)); // пустой
    }
    Vector* current_result = lists[0];
    for (int i = 1; i < n; i++) {
        Vector* next_result = intersectTwoPostings(current_result, lists[i]);
        if (current_result != lists[0]) {
            vectorFree(current_result); 
        }
        current_result = next_result;
        if (current_result -> size == 0) {
            break;
        }
    }
    return current_result;
}

int compareVectorsBySize(const void* a, const void* b) {
    Vector* v1 = *(Vector**)a;
    Vector* v2 = *(Vector**)b;
    return v1 -> size - v2 -> size;
}

int compareSearchResults(const void* a, const void* b) {
    SearchResult* r1 = (SearchResult*)a;
    SearchResult* r2 = (SearchResult*)b;
    return r2 -> score - r1 -> score;
}

SearchResults* search(Index* idx, const char* query) {
    clock_t start_time = clock();
    SearchResults* sr = (SearchResults*)malloc(sizeof(SearchResults));
    if (sr == NULL) {
        printf("memory failed");
        return NULL;
    }
    sr -> results = createVector(sizeof(SearchResult));
    sr -> time_ms = 0.0;
    sr -> total = 0;
    if (query == NULL || strlen(query) == 0) {
        printf("request is empty");
        return sr;
    }
    char* query_copy = strdup(query); // копия строки
    for (int i = 0; query_copy[i]; i++) {
        query_copy[i] = tolower((unsigned char)query_copy[i]); // приводим к нижнему регистру 
    }
    char* tokens[MAX_TOKENS];
    int n_tokens = 0;
    // \t режет по табам \n пон \r каретка, далее пуктуация 
    char* token = strtok(query_copy, " \t\n\r.,!?();:\"");
    while (token != NULL && n_tokens < MAX_TOKENS) {
        tokens[n_tokens++] = token;
        token = strtok(NULL, " \t\n\r.,!?();:\""); // передаем NULL, тк после каждого вызова функции она запоминает на каком моменте остановилась
    }
    if (n_tokens == 0) {
        free(query_copy);
        return sr;
    }
    Vector** posting_lists = (Vector**)malloc(n_tokens * sizeof(Vector*));
    for (int i = 0; i < n_tokens; i++) {
        posting_lists[i] = lookupTerm(idx, tokens[i]);
        // если хотя бы одно слово не найдено, пересечение будет пустым
        if (posting_lists[i] == NULL || posting_lists[i] -> size == 0) {
            free(posting_lists);
            free(query_copy);
            sr -> time_ms = ((double)(clock() - start_time) / CLOCKS_PER_SEC) * 1000.0;
            return sr;
        }
    }
    qsort(posting_lists, n_tokens, sizeof(Vector*), compareVectorsBySize); // чтобы самые короткие массивы оказались вначале
    Vector* intersected = intersectPostings(posting_lists, n_tokens);
    sr -> total = intersected -> size;
    if (sr -> total > 0) {
        // временный массив для сортировки
        SearchResult* temp_results = (SearchResult*)malloc(sr -> total * sizeof(SearchResult));
        // копируем всё из вектора
        for (int i = 0; i < sr -> total; i++) {
            values_from_keys* val = (values_from_keys*)getVectorItem(intersected, i);
            temp_results[i].doc_id = val -> doc_id;
            temp_results[i].score = n_tokens; 
            strncpy(temp_results[i].title, val -> title, MAX_TITLE_LEN - 1);
            temp_results[i].title[MAX_TITLE_LEN - 1] = '\0';
        }
        // сортируем массив, чтобы все дубликаты были подряд 
        qsort(temp_results, sr -> total, sizeof(SearchResult), compareSearchResults);
        // считаем реальное количество уникальных документов
        int unique_total = 0;
        int last_id_seen = -1;
        for (int i = 0; i < sr -> total; i++) {
            if (temp_results[i].doc_id != last_id_seen) {
                unique_total++;
                last_id_seen = temp_results[i].doc_id;
            }
        }
        // добавляем в финальный результат только уникальные
        int added = 0;
        last_id_seen = -1; // сбрасываем для второго прохода
        
        for (int i = 0; i < sr -> total; i++) {
            if (added >= 10) {
                break;
            }
            // если документ еще не добавляли
            if (temp_results[i].doc_id != last_id_seen) {
                appendVectorItem(sr -> results, &temp_results[i]);
                last_id_seen = temp_results[i].doc_id;
                added++;
            }
        }
        sr -> total = unique_total;
        free(temp_results);
    }
    if (intersected != NULL && intersected != posting_lists[0]) {
        vectorFree(intersected);
    }
    free(posting_lists);
    free(query_copy);

    sr -> time_ms = ((double)(clock() - start_time) / CLOCKS_PER_SEC) * 1000.0;
    return sr;
}

void printResultsText(const SearchResults* sr) {
    if (sr == NULL) {
        printf("search result is NULL\n");
        return;
    }
    printf("documents: %d (search time: %.2f ms)\n", sr -> total, sr -> time_ms);
    if (sr -> results == NULL || sr -> results -> size == 0) {
        printf("funny request, cant find it\n");
    } else {
        for (int i = 0; i < sr -> results -> size; i++) {
            SearchResult* res = (SearchResult*)getVectorItem(sr -> results, i);
            printf("%d. [ID: %d] (Score: %d) %s\n", 
                   i + 1, 
                   res -> doc_id, 
                   res -> score, 
                   res -> title);
        }
    }
}

void printResultsJSON(const SearchResults* sr) {
    if (sr == NULL) {
        printf("{}\n");
        return;
    }
    printf("{\n");
    printf("  \"total\": %d,\n", sr -> total);
    printf("  \"time_ms\": %.2f,\n", sr -> time_ms);
    printf("  \"results\": [\n");
    if (sr -> results != NULL) {
        for (int i = 0; i < sr -> results -> size; i++) {
            SearchResult* res = (SearchResult*)getVectorItem(sr -> results, i);
            printf("    {\n");
            printf("      \"doc_id\": %d,\n", res -> doc_id);
            printf("      \"score\": %d,\n", res -> score);
            printf("      \"title\": \"%s\"\n", res -> title);
            if (i < sr -> results->size - 1) {
                printf("    },\n");
            } else {
                printf("    }\n");
            }
        }
    }
    printf("  ]\n");
    printf("}\n");
}

void freeSearchResults(SearchResults* sr) {
    if (sr != NULL) {
        if (sr -> results != NULL) {
            vectorFree(sr -> results);
        }
        free(sr);
    }
}