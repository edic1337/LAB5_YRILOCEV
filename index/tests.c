#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "index.h"
#include "search.h"
#include "../btree/btree.h"
#include "../avl/avl.h"
#include "../rbtree/rbtree.h"

#define RUN_TEST(test_func) \
    do { \
        printf("[RUN] %s...\n", #test_func); \
        test_func(); \
        printf("[OK]  %s passed!\n\n", #test_func); \
    } while(0)

void test_create_and_free_index() {
    // Проверяем создание
    Index* idx = createIndex(TREE_AVL);
    assert(idx != NULL);
    assert(idx -> tree != NULL);
    
    // Проверяем очистку
    freeIndex(idx);
}

void test_insert_and_lookup() {
    Index* idx = createIndex(TREE_AVL);
    assert(idx != NULL);

    insertTerm(idx, "hello", 1, "Document One");
    insertTerm(idx, "world", 1, "Document One");
    insertTerm(idx, "hello", 2, "Document Two");

    // Ищем существующий термин (ожидаем 2 документа)
    Vector* postings_hello = lookupTerm(idx, "hello");
    assert(postings_hello != NULL);
    assert(postings_hello -> size == 2);

    values_from_keys* val1 = (values_from_keys*)getVectorItem(postings_hello, 0);
    assert(val1 -> doc_id == 1 || val1 -> doc_id == 2); // Зависит от реализации дерева/вектора

    // Ищем несуществующий термин
    Vector* postings_none = lookupTerm(idx, "ghost");
    assert(postings_none == NULL || postings_none -> size == 0);

    freeIndex(idx);
}

void test_index_document() {
    Index* idx = createIndex(TREE_BTREE);
    
    const char* tokens[] = {"apple", "banana", "orange"};
    indexDocument(idx, 10, "Fruit Basket", tokens, 3);

    Vector* res = lookupTerm(idx, "banana");
    assert(res != NULL);
    assert(res -> size == 1);
    
    values_from_keys* val = (values_from_keys*)getVectorItem(res, 0);
    assert(val -> doc_id == 10);
    assert(strcmp(val -> title, "Fruit Basket") == 0);

    freeIndex(idx);
}

void test_save_and_load_index() {
    const char* test_file = "test_index_dump.txt";
    Index* idx = createIndex(TREE_RB);
    
    insertTerm(idx, "testword", 42, "Test Title");
    saveIndex(idx, test_file);
    freeIndex(idx);

    // Восстанавливаем индекс
    Index* loaded_idx = loadIndex(test_file, TREE_RB);
    assert(loaded_idx != NULL);

    Vector* res = lookupTerm(loaded_idx, "testword");
    assert(res != NULL);
    assert(res -> size == 1);

    values_from_keys* val = (values_from_keys*)getVectorItem(res, 0);
    assert(val -> doc_id == 42);

    freeIndex(loaded_idx);
    remove(test_file); // Удаляем временный файл
}

void test_search_single_word() {
    Index* idx = createIndex(TREE_AVL);
    insertTerm(idx, "cat", 1, "About Cats");
    insertTerm(idx, "cat", 2, "Cats and Dogs");

    SearchResults* sr = search(idx, "cat");
    assert(sr != NULL);
    assert(sr -> total == 2);
    assert(sr -> results->size == 2);

    freeSearchResults(sr);
    freeIndex(idx);
}

void test_search_multiple_words_intersection() {
    Index* idx = createIndex(TREE_AVL);
    
    const char* doc1[] = {"cat", "dog", "mouse"};
    indexDocument(idx, 1, "Doc 1", doc1, 3);

    const char* doc2[] = {"cat", "bird"};
    indexDocument(idx, 2, "Doc 2", doc2, 2);

    SearchResults* sr = search(idx, "cat dog");
    assert(sr != NULL);
    assert(sr -> total == 1);
    
    SearchResult* res = (SearchResult*)getVectorItem(sr -> results, 0);
    assert(res -> doc_id == 1);

    freeSearchResults(sr);
    freeIndex(idx);
}

void test_search_empty_or_not_found() {
    Index* idx = createIndex(TREE_AVL);
    insertTerm(idx, "hello", 1, "Title");

    // Пустой запрос
    SearchResults* sr_empty = search(idx, "");
    assert(sr_empty != NULL);
    assert(sr_empty -> total == 0);
    freeSearchResults(sr_empty);

    // Запрос несуществующего слова
    SearchResults* sr_not_found = search(idx, "world");
    assert(sr_not_found != NULL);
    assert(sr_not_found -> total == 0);
    freeSearchResults(sr_not_found);

    // Пересечение существующего и несуществующего (должен быть 0)
    SearchResults* sr_intersect_empty = search(idx, "hello world");
    assert(sr_intersect_empty != NULL);
    assert(sr_intersect_empty -> total == 0);
    freeSearchResults(sr_intersect_empty);

    freeIndex(idx);
}

int main() {
    printf("UNIT TESTS\n\n");

    RUN_TEST(test_create_and_free_index);
    RUN_TEST(test_insert_and_lookup);
    RUN_TEST(test_index_document);
    RUN_TEST(test_save_and_load_index);
    
    RUN_TEST(test_search_single_word);
    RUN_TEST(test_search_multiple_words_intersection);
    RUN_TEST(test_search_empty_or_not_found);

    printf("TESTS PASSED\n");
    return 0;
}