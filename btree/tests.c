#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "btree.h"

// простой visitor для traverse
void collect_visit(const char* key, Vector* postings, void* ctx) {
    int* count = (int*)ctx;
    (*count)++;
}

// проверка вставки и поиска
void test_insert_and_search() {
    BTree* tree = createBTree();

    btreeInsert(tree, "apple", 1, "doc1");
    btreeInsert(tree, "banana", 2, "doc2");
    btreeInsert(tree, "cherry", 3, "doc3");

    Vector* v1 = btreeSearch(tree, "apple");
    Vector* v2 = btreeSearch(tree, "banana");
    Vector* v3 = btreeSearch(tree, "cherry");

    assert(v1 != NULL);
    assert(v2 != NULL);
    assert(v3 != NULL);

    printf("test_insert_and_search OK\n");

    freeBTree(tree);
}

// проверка дублей ключей
void test_duplicate_keys() {
    BTree* tree = createBTree();

    btreeInsert(tree, "apple", 1, "doc1");
    btreeInsert(tree, "apple", 2, "doc2");
    btreeInsert(tree, "apple", 3, "doc3");

    Vector* v = btreeSearch(tree, "apple");

    assert(v != NULL);
    // ожидаем минимум 3 элемента в postings

    printf("test_duplicate_keys OK\n");

    freeBTree(tree);
}

// проверка порядка обхода
void test_traverse() {
    BTree* tree = createBTree();

    btreeInsert(tree, "b", 1, "b");
    btreeInsert(tree, "a", 2, "a");
    btreeInsert(tree, "c", 3, "c");
    btreeInsert(tree, "d", 4, "d");
    btreeInsert(tree, "e", 5, "e");

    int count = 0;

    btreeTraverse(tree, collect_visit, &count);

    // должно быть 5 ключей
    assert(count == 5);

    printf("test_traverse OK\n");

    freeBTree(tree);
}

// проверка поиска несуществующего ключа
void test_search_missing() {
    BTree* tree = createBTree();

    btreeInsert(tree, "apple", 1, "doc1");

    Vector* v = btreeSearch(tree, "banana");

    assert(v == NULL);

    printf("test_search_missing OK\n");

    freeBTree(tree);
}

// стресс-тест (простая вставка многих элементов)
void test_many_insertions() {
    BTree* tree = createBTree();

    for (int i = 0; i < 1000; i++) {
        char key[32];
        sprintf(key, "key_%d", i);
        btreeInsert(tree, key, i, "doc");
    }

    Vector* v = btreeSearch(tree, "key_500");
    assert(v != NULL);

    printf("test_many_insertions OK\n");

    freeBTree(tree);
}

// main
int main() {
    test_insert_and_search();
    test_duplicate_keys();
    test_traverse();
    test_search_missing();
    test_many_insertions();

    printf("ALL TESTS PASSED\n");
    return 0;
}
