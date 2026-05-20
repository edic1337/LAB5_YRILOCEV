#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "index/index.h"
#include "index/search.h"

// переводит строковое название типа дерева во внутреннее перечисление TreeType
TreeType parseType(const char* type_str) {
    if (strcmp(type_str, "avl") == 0) {
        return TREE_AVL;
    }
    if (strcmp(type_str, "rbtree") == 0) {
        return TREE_RB;
    }
    if (strcmp(type_str, "btree") == 0) {
        return TREE_BTREE;
    }
    
    printf("warning: type tree is unknown '%s', use avl type\n", type_str);
    return TREE_AVL;
}

// делает обратное, переводит TreeType в строку
const char* typeName(TreeType type) {
    switch (type) {
        case TREE_AVL:
            return "avl";
        case TREE_RB:
            return "rbtree";
        case TREE_BTREE:
            return "btree";
        default:
            return "unknown";
    }
}

// Отвечает за весь процесс создания индекса
static void runIndex(TreeType type, const char* data_path, const char* idx_path) {
    
    printf("run index process\n");
    printf("tree type: %s\n", typeName(type));
    printf("data files: %s\n", data_path);
    printf("result files: %s\n", idx_path);

    Index* idx = createIndex(type);
    if (idx == NULL) {
        printf("memory failed\n");
        exit(1);
    }

    printf("data parsing and building tree\n");
    buildIndex(idx, data_path);
    printf("saved tree\n");
    saveIndex(idx, idx_path);
    freeIndex(idx);
    printf("index created\n");
}

// отвечает за поиск по готовому индексу
static void runSearch(TreeType type, const char* idx_path, const char* query, int json_out) {
    Index* idx = loadIndex(idx_path, type);
    if (!idx) { 
        printf("failed load index\n"); 
        exit(1); 
    }
    
    SearchResults* sr = search(idx, query);
    
    if (json_out) {
        printResultsJSON(sr);
    } else {
        printResultsText(sr);
    }
    
    freeSearchResults(sr);
    freeIndex(idx);
}


int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("please, give me arguments");
        return 1;
    }

    const char* mode = argv[1]; // режим работы "index" или "search"
    TreeType type = TREE_AVL;   // тип дерева по умолчанию
    const char* data_path = "data/processed/docs.jsonl"; // файл с данными по умолчанию
    char idx_path[512] = {0};   // пустой массив для пути к файлу индекса
    int json_out = 0;           // флаг вывода в JSON
    const char* query = NULL;   // поисковой запрос

    // Цикл перебирает все аргументы консоли, начиная с третьего
    for (int i = 2; i < argc; i++) {
        // аргумент начинается с "--type="
        if (strncmp(argv[i], "--type=", 7) == 0) {
            type = parseType(argv[i] + 7); // пропускаем первые 7 символов и читаем тип
        }
        // аргумент начинается с "--data="
        else if (strncmp(argv[i], "--data=", 7) == 0) {
            data_path = argv[i] + 7;       // сохраняем путь к данным
        }
        // аргумент начинается с "--index="
        else if (strncmp(argv[i], "--index=", 8) == 0) {
            strncpy(idx_path, argv[i] + 8, sizeof(idx_path) - 1); // копируем путь к индексу
        }
        // аргумент точно совпадает с "--json"
        else if (strcmp(argv[i], "--json") == 0) {
            json_out = 1;                  // включаем флаг вывода в формате JSON
        }
        // если аргумент не начинается с дефиса, считаем, что это сам текст запроса
        else if (argv[i][0] != '-') {
            query = argv[i];
        }
    }
    // пользователь не указал путь к индексу вручную (массив остался пустым), мы генерируем стандартное имя файла
    if (idx_path[0] == '\0') {
        snprintf(idx_path, sizeof(idx_path), "data/index_%s.txt", typeName(type));
    }
    // "index" - запускаем индексацию
    if (strcmp(mode, "index") == 0) {
        runIndex(type, data_path, idx_path);
    } 
    // "search" - проверяем наличие запроса и запускаем поиск
    else if (strcmp(mode, "search") == 0) {
        if (!query) { 
            printf("failed: please, give me correct response\n"); 
            return 1; 
        }
        runSearch(type, idx_path, query, json_out);
    } 
    else {
        printf("sorry, inkown work type'%s'\n", mode);
        return 1;
    }
    return 0;
}