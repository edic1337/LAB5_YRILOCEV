#include <assert.h>
#include <stdio.h>
#include "rbtree.h"

void log_result(const char* task, const char* message) {
    printf("[%s] %s\n", task, message);
}

// вспомогательная структура для проверки порядка обхода
typedef struct {
    const char* expected_keys[10];
    int current_index;
    int total_count;
} TraverseContext;

// для проверки порядка 
void test_traverse_order(const char* key, Vector* postings, void* ctx) {
    TraverseContext* tc = (TraverseContext*)ctx;
    // Проверяем, что текущий ключ соответствует ожидаемому в алфавитном порядке
    assert(strcmp(key, tc->expected_keys[tc->current_index]) == 0);
    tc->current_index++;
    tc->total_count++;
}

// для тестирования обхода
void test_visit(const char* key, Vector* postings, void* ctx) {
    int* count = (int*)ctx;
    (*count)++;
}

int main() {
    
    log_result("создание", "Начинаем тестирование");
    RBTree* tree = createRBTree();
    assert(tree != NULL);
    assert(tree->root == tree->nil);
    assert(tree->size == 0);
    freeRBTree(tree);
    log_result("Создание", "Успешно пройдено");



    log_result("Вставка и Поиск", "Начинаем тестирование");

    RBTree* tree = createRBTree();

    // стандартная вставка
    rbInsert(tree, "zebra", 10, "ZebraDoc");
    rbInsert(tree, "apple", 1, "AppleDoc");
    rbInsert(tree, "mango", 5, "MangoDoc");
    
    assert(rbSearch(tree, "zebra") != NULL);
    assert(rbSearch(tree, "apple") != NULL);
    assert(rbSearch(tree, "mango") != NULL);
    assert(tree->size == 3);

    // тест на дубликаты (один и тот же ключ, разные документы)
    rbInsert(tree, "apple", 2, "AppleDoc2");
    Vector* v_apple = rbSearch(tree, "apple");
    assert(v_apple != NULL);
    assert(getPostingCount(v_apple) == 2); 
    assert(tree->size == 3); // Размер дерева не изменился

    // проверка содержимого (вставились ли правильные данные)
    assert(strcmp(getPostingTitle(v_apple, 0), "AppleDoc") == 0);
    assert(strcmp(getPostingTitle(v_apple, 1), "AppleDoc2") == 0);

    // тест на вставку в "отсортированном" порядке (вызывает балансировки)
    rbInsert(tree, "a", 1, "D1");
    rbInsert(tree, "b", 2, "D2");
    rbInsert(tree, "c", 3, "D3");
    rbInsert(tree, "d", 4, "D4");
    assert(tree->size == 7);
    assert(rbSearch(tree, "d") != NULL);

    // поиск после множественных вставок
    assert(getPostingCount(rbSearch(tree, "a")) == 1);
    assert(getPostingCount(rbSearch(tree, "b")) == 1);

    // тест на несуществующие ключи
    assert(rbSearch(tree, "banana") == NULL);
    assert(rbSearch(tree, "KEY1") == NULL); // Регистрозависимость
    assert(rbSearch(tree, "non_existent") == NULL);

    // тест на пустой ключ ""
    rbInsert(tree, "", 999, "EmptyKeyTitle");
    Vector* v_empty = rbSearch(tree, "");
    assert(v_empty != NULL);
    assert(strcmp(getPostingTitle(v_empty, 0), "EmptyKeyTitle") == 0);

    // тест на некорректные данные (NULL вставка)
    rbInsert(tree, NULL, 0, "Broken"); // Должно игнорироваться
    assert(tree->size == 8); // Было 7, добавили "" (пустую строку), итого 8

    // тест на поиск по NULL
    assert(rbSearch(tree, NULL) == NULL);

    freeRBTree(tree);

    log_result("Вставка и Поиск", "Все тесты успешно пройдены");




    log_result("Дубликаты ключей", "Начинаем расширенное тестирование");

    RBTree* tree = createRBTree();

    // вставка первого элемента
    rbInsert(tree, "apple", 1, "First");
    assert(tree->size == 1);
    
    // многократные дубликаты одного ключа
    rbInsert(tree, "apple", 2, "Second");
    rbInsert(tree, "apple", 3, "Third");
    rbInsert(tree, "apple", 4, "Fourth");
    
    // размер дерева остался прежним
    assert(tree->size == 1);
    
    // вектор содержит 4 элемента
    Vector* v = rbSearch(tree, "apple");
    assert(v != NULL);
    assert(getPostingCount(v) == 4);
    
    // данные сохранены в правильном порядке 
    assert(strcmp(getPostingTitle(v, 0), "First") == 0);
    assert(strcmp(getPostingTitle(v, 1), "Second") == 0);
    assert(strcmp(getPostingTitle(v, 2), "Third") == 0);
    assert(strcmp(getPostingTitle(v, 3), "Fourth") == 0);

    // смешанные дубликаты (вставляем между дубликатами другие ключи)
    rbInsert(tree, "banana", 5, "BananaDoc");
    rbInsert(tree, "apple", 6, "Fifth"); // Снова дубликат "apple"
    
    assert(tree->size == 2); // Только "apple" и "banana"
    
    Vector* v_apple = rbSearch(tree, "apple");
    assert(getPostingCount(v_apple) == 5);
    assert(strcmp(getPostingTitle(v_apple, 4), "Fifth") == 0);
    
    Vector* v_banana = rbSearch(tree, "banana");
    assert(getPostingCount(v_banana) == 1);
    assert(strcmp(getPostingTitle(v_banana, 0), "BananaDoc") == 0);

    // проверка на дубликат ключа, который уже находится глубоко в дереве (после балансировки)
    // вставляем ключи, чтобы спровоцировать балансировку
    const char* keys[] = {"a", "b", "c", "d", "e"};
    for(int i = 0; i < 5; i++) {
        rbInsert(tree, keys[i], i, "temp");
    }
    
    // теперь добавляем дубликат к "c" (который может быть корнем или узлом)
    rbInsert(tree, "c", 99, "C_Duplicate");
    Vector* v_c = rbSearch(tree, "c");
    assert(getPostingCount(v_c) == 2);
    assert(strcmp(getPostingTitle(v_c, 1), "C_Duplicate") == 0);

    freeRBTree(tree);

    log_result("Дубликаты ключей", "Все тесты на дубликаты пройдены");




    log_result("Обход дерева", "Начинаем тестирование");

    RBTree* tree = createRBTree();
    
    // вставляем ключи в хаотичном порядке
    rbInsert(tree, "mango", 1, "Doc");
    rbInsert(tree, "apple", 1, "Doc");
    rbInsert(tree, "zebra", 1, "Doc");
    rbInsert(tree, "banana", 1, "Doc");

    // проверка количества (базовый тест)
    int count = 0;
    rbTraverse(tree, test_visit, &count); // используем ваш простой счетчик
    assert(count == 4);

    // проверка порядка ( apple, banana, mango, zebra)
    TraverseContext tc = {
        .expected_keys = {"apple", "banana", "mango", "zebra"},
        .current_index = 0,
        .total_count = 0
    };
    
    rbTraverse(tree, test_traverse_order, &tc);
    assert(tc.total_count == 4);


    // обход пустого дерева (не должно вызвать падений)
    RBTree* emptyTree = createRBTree();
    int emptyCount = 0;
    rbTraverse(emptyTree, test_visit, &emptyCount);
    assert(emptyCount == 0);

    freeRBTree(tree);
    freeRBTree(emptyTree);

    log_result("Обход дерева(разные случаи)", "Успешно");




    log_result("Тестирование поиска несуществующих ключей(негативные сценарии)", "Начинаем тестирование");

    RBTree* tree = createRBTree();
    
    rbInsert(tree, "cat", 1, "D1");
    rbInsert(tree, "dog", 1, "D2");
    rbInsert(tree, "fox", 1, "D3");

    // поиск ключа, которого точно нет
    assert(rbSearch(tree, "durian") == NULL);
    
    // поиск "между" существующими ключами (alphabetically)
    // "cat" < "cow" < "dog", но "cow" не вставлен
    assert(rbSearch(tree, "cow") == NULL);
    
    // поиск ключа "меньше" минимального (apple < cat)
    assert(rbSearch(tree, "apple") == NULL);
    
    // поиск ключа "больше" максимального (zebra > fox)
    assert(rbSearch(tree, "zebra") == NULL);
    
    // поиск по пустой строке (если дерево не пустое, но "" нет)
    assert(rbSearch(tree, "") == NULL);

    // поиск другого регистра (чувствительность к регистру)
    // в дереве "cat", ищем "CAT" или "Cat"
    assert(rbSearch(tree, "CAT") == NULL);
    assert(rbSearch(tree, "Cat") == NULL);

    // поиск слишком длинного ключа
    assert(rbSearch(tree, "long_non_existent_key_that_is_very_long") == NULL);

    // поиск в пустом дереве (проверка на случай, если tree->root == tree->nil)
    RBTree* emptyTree = createRBTree();
    assert(rbSearch(emptyTree, "anything") == NULL);
    freeRBTree(emptyTree);
    freeRBTree(tree);

    log_result("Поиск (негативные сценарии)", "Все тесты успешно пройдены");



    log_result("Пустое дерево", "Начинаем тестирование");

    RBTree* emptyTree = createRBTree();
    
    // проверка размера после создания
    assert(emptyTree->size == 0);
    assert(emptyTree->root == emptyTree->nil);

    // поиск в пустом дереве
    assert(rbSearch(emptyTree, "anything") == NULL);
    assert(rbSearch(emptyTree, "") == NULL);
    assert(rbSearch(emptyTree, NULL) == NULL);

    // обход пустого дерева (не должен ничего вызывать)
    int emptyCount = 0;
    rbTraverse(emptyTree, test_visit, &emptyCount);
    assert(emptyCount == 0);

    // попытка вставки в пустое дерево и последующий поиск
    rbInsert(emptyTree, "solo", 1, "OnlyDoc");
    assert(emptyTree->size == 1);
    assert(rbSearch(emptyTree, "solo") != NULL);
    
    freeRBTree(emptyTree);
    
    log_result("Пустое дерево", "Все тесты успешно пройдены");

    printf("\nВсе тесты для RBTREE пройдены успешно!\n");
    return 0;
}