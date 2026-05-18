#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "avl.h"
#include "../posting.h"
#include "../lab3/vector/generic.h"
#include <windows.h>

// логирование результатов
void log_result(const char* task, const char* message) {
    printf("[%s] %s\n", task, message);
}

// вспомогательная структура для проверки порядка обхода
typedef struct {
    const char* expected_keys[100];
    int current_index;
    int total_count;
} TraverseContext;

// получение размера вектора
size_t getPostingCount(Vector* v) {
    if (v == NULL) {
        return 0;
    }
    return v->size;
}

// получение заголовка
const char* getPostingTitle(Vector* v, size_t index) {
    if (v == NULL) {
        return NULL;
    }
    PostingEntry* entry = (PostingEntry*)getVectorItem(v, index);
    if (entry == NULL) {
        return NULL;
    }
    return entry->title; 
}

// callback для проверки алфавитного порядка
void test_traverse_order(const char* key, Vector* postings, void* ctx) {
    TraverseContext* tc = (TraverseContext*)ctx;
    if (strcmp(key, tc->expected_keys[tc->current_index]) != 0) {
        printf("Ошибка порядка! Ждали: %s, получили: %s\n", tc->expected_keys[tc->current_index], key);
    }
    assert(strcmp(key, tc->expected_keys[tc->current_index]) == 0);
    tc->current_index = tc->current_index + 1;
    tc->total_count = tc->total_count + 1;
}

// Простой счетчик для посещений
void test_visit_simple(const char* key, Vector* postings, void* ctx) {
    int* count = (int*)ctx;
    if (count != NULL) {
        *count = *count + 1;
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    // ============================================================
    log_result("Тест 1: создание пустого дерева", "- начало");
    AVLTree* tree = createAVLTree();
    assert(tree != NULL);
    assert(tree->root == NULL);
    assert(tree->size == 0);
    
    assert(avlSearch(tree, "anything") == NULL);
    int count = 0;
    avlTraverse(tree, test_visit_simple, &count);
    assert(count == 0);
    
    freeAVLTree(tree);
    log_result("Тест 1", "- успешно пройдено");


    log_result("Тест 2: добавление новых слов (с дубликатами)", "- начало");
    tree = createAVLTree();

    avlInsert(tree, "zebra", 10, "ZebraDoc");
    avlInsert(tree, "apple", 1, "AppleDoc");
    avlInsert(tree, "mango", 5, "MangoDoc");
    
    assert(tree->size == 3);
    assert(avlSearch(tree, "zebra") != NULL);
    assert(avlSearch(tree, "apple") != NULL);
    
    // Тест на дубликаты (один ключ, много документов)
    avlInsert(tree, "apple", 2, "Apple_V2");
    avlInsert(tree, "apple", 3, "Apple_V3");
    
    Vector* v_apple = avlSearch(tree, "apple");
    assert(getPostingCount(v_apple) == 3);
    assert(tree->size == 3); // Уникальных ключей всё еще 3
    assert(strcmp(getPostingTitle(v_apple, 2), "Apple_V3") == 0);

    freeAVLTree(tree);
    log_result("Тест 2", "- успешно пройдено");


    log_result("Тест 3: проверка поворотов", "- начало");
    tree = createAVLTree();

    // Случай LL
    avlInsert(tree, "30", 1, "D30");
    avlInsert(tree, "20", 1, "D20");
    avlInsert(tree, "10", 1, "D10");
    assert(tree->size == 3);
    assert(avlSearch(tree, "20") != NULL);

    // Случай RR
    avlInsert(tree, "40", 1, "D40");
    avlInsert(tree, "50", 1, "D50");
    assert(avlSearch(tree, "40") != NULL);

    // Случай LR
    freeAVLTree(tree);
    tree = createAVLTree();
    avlInsert(tree, "50", 1, "D50");
    avlInsert(tree, "20", 1, "D20");
    avlInsert(tree, "40", 1, "D40"); 
    assert(tree->size == 3);
    assert(avlSearch(tree, "40") != NULL);

    // Случай RL
    avlInsert(tree, "70", 1, "D70");
    avlInsert(tree, "60", 1, "D60");
    assert(avlSearch(tree, "60") != NULL);

    freeAVLTree(tree);
    log_result("Тест 3", "- успешно пройдено");


    log_result("Тест 4: проверка алфавитного обхода", "- начало");
    tree = createAVLTree();
    
    avlInsert(tree, "cherry", 1, "C");
    avlInsert(tree, "apple", 1, "A");
    avlInsert(tree, "banana", 1, "B");
    avlInsert(tree, "date", 1, "D");
    avlInsert(tree, "", 1, "Empty"); // Пустая строка как ключ
    avlInsert(tree, "123", 1, "Numbers");

    // Ожидаемый алфавитный порядок: "", "123", "apple", "banana", "cherry", "date"
    TraverseContext tc = {
        .expected_keys = {"", "123", "apple", "banana", "cherry", "date"},
        .current_index = 0,
        .total_count = 0
    };
    
    avlTraverse(tree, test_traverse_order, &tc);
    assert(tc.total_count == 6);

    freeAVLTree(tree);
    log_result("Тест 4", "- успешно пройдено");


    log_result("Тест 5: проверка регистра и NULL", "- начало");
    tree = createAVLTree();
    
    avlInsert(tree, "Word", 1, "CaseTest");
    
    assert(avlSearch(tree, "word") == NULL); // Регистр важен
    assert(avlSearch(tree, "WORD") == NULL);
    assert(avlSearch(tree, "Wor") == NULL);  // Часть слова
    assert(avlSearch(tree, "Wordy") == NULL); // Лишние буквы
    
    // NULL входы
    avlInsert(NULL, "test", 1, "T");
    avlInsert(tree, NULL, 1, "T");
    assert(avlSearch(NULL, "Word") == NULL);
    assert(avlSearch(tree, NULL) == NULL);

    freeAVLTree(tree);
    log_result("Тест 5", "- успешно пройдено");


    log_result("Тест 6: Стресс-тест (вставка 1000 элементов)", "- начало");
    tree = createAVLTree();
    
    for (int i = 0; i < 1000; i++) {
        char key[20];
        sprintf(key, "key_%04d", i);
        avlInsert(tree, key, i, "StressDoc");
    }
    
    assert(tree->size == 1000);
    assert(avlSearch(tree, "key_0500") != NULL);
    assert(avlSearch(tree, "key_0999") != NULL);
    assert(getPostingCount(avlSearch(tree, "key_0000")) == 1);
    
    // Проверка, что после 1000 вставок обход всё еще работает
    int stress_count = 0;
    avlTraverse(tree, test_visit_simple, &stress_count);
    assert(stress_count == 1000);

    freeAVLTree(tree);
    log_result("Тест 6", "- успешно пройдено");


    log_result("Тест 7: много документов на одно слово", "- начало");
    tree = createAVLTree();
    
    for (int i = 0; i < 500; i++) {
        avlInsert(tree, "constant", i, "RepeatDoc");
    }
    
    assert(tree->size == 1);
    Vector* v_const = avlSearch(tree, "constant");
    assert(getPostingCount(v_const) == 500);
    assert(strcmp(getPostingTitle(v_const, 499), "RepeatDoc") == 0);

    freeAVLTree(tree);
    log_result("Тест 7", "- успешно пройдено");


    printf("Все тесты для AVL дерева пройдены успешно!\n");

    return 0;
}