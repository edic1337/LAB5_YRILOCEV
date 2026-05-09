#include "btree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Простые цвета для Windows/Linux
#ifdef _WIN32
    #define RESET   ""
    #define RED     ""
    #define GREEN   ""
    #define YELLOW  ""
    #define BLUE    ""
    #define MAGENTA ""
    #define CYAN    ""
    #define BOLD    ""
#else
    #define RESET   "\033[0m"
    #define RED     "\033[31m"
    #define GREEN   "\033[32m"
    #define YELLOW  "\033[33m"
    #define BLUE    "\033[34m"
    #define MAGENTA "\033[35m"
    #define CYAN    "\033[36m"
    #define BOLD    "\033[1m"
#endif

// Функция для красивого вывода дерева
void printTree(BTreeNode* node, int level, char* prefix) {
    if (!node) return;
    
    // Отступы
    for (int i = 0; i < level; i++) {
        printf("  ");
    }
    printf("%s", prefix);
    
    // Вывод ключей в узле
    printf("[");
    for (int i = 0; i < node->key_count; i++) {
        if (i > 0) printf(", ");
        printf("%s", node->keys[i]);
        if (node->postings[i] && node->postings[i]->size > 0) {
            printf("(%d)", node->postings[i]->size);
        }
    }
    printf("]");
    
    if (node->is_leaf) {
        printf(" %s[ЛИСТ]%s\n", CYAN, RESET);
    } else {
        printf(" %s[ВНУТР]%s\n", MAGENTA, RESET);
    }
    
    // Рекурсивный вывод детей
    if (!node->is_leaf) {
        for (int i = 0; i <= node->key_count; i++) {
            char new_prefix[256];
            if (i == node->key_count) {
                snprintf(new_prefix, sizeof(new_prefix), "└── ");
            } else {
                snprintf(new_prefix, sizeof(new_prefix), "├── ");
            }
            printTree(node->children[i], level + 1, new_prefix);
        }
    }
}

// Функция для отображения всех ключей в дереве (обход)
void printAllKeys(BTreeNode* node, int* first) {
    if (!node) return;
    
    if (node->is_leaf) {
        for (int i = 0; i < node->key_count; i++) {
            if (!(*first)) printf(", ");
            printf("\"%s\"", node->keys[i]);
            *first = 0;
        }
    } else {
        for (int i = 0; i <= node->key_count; i++) {
            printAllKeys(node->children[i], first);
            if (i < node->key_count) {
                if (!(*first)) printf(", ");
                printf("\"%s\"", node->keys[i]);
                *first = 0;
            }
        }
    }
}

// Функция для подсчета количества узлов в дереве
int countNodes(BTreeNode* node) {
    if (!node) return 0;
    int count = 1;
    if (!node->is_leaf) {
        for (int i = 0; i <= node->key_count; i++) {
            count += countNodes(node->children[i]);
        }
    }
    return count;
}

// Функция для подсчета всех ключей
int countAllKeys(BTreeNode* node) {
    if (!node) return 0;
    int count = node->key_count;
    if (!node->is_leaf) {
        for (int i = 0; i <= node->key_count; i++) {
            count += countAllKeys(node->children[i]);
        }
    }
    return count;
}

// ============== ТЕСТЫ ==============

// ТЕСТ 1: Базовые операции (очень простой)
void test1_basic() {
    printf("\n%s════════════════════════════════════════════%s\n", BOLD, RESET);
    printf("%s📝 ТЕСТ 1: Базовые операции%s\n", BOLD, RESET);
    printf("%s════════════════════════════════════════════%s\n\n", BOLD, RESET);
    
    BTree* tree = createBTree();
    
    printf("1. Вставляем слово 'apple' (документ 1)\n");
    btreeInsert(tree, "apple", 1, "Apple Guide");
    
    printf("2. Ищем 'apple': ");
    Vector* result = btreeSearch(tree, "apple");
    if (result && result->size > 0) {
        values_from_keys* v = (values_from_keys*)getVectorItem(result, 0);
        printf("%sНАЙДЕНО!%s (doc %d: %s)\n", GREEN, RESET, v->doc_id, v->title);
    } else {
        printf("%sНЕ НАЙДЕНО%s\n", RED, RESET);
    }
    vectorFree(result);
    
    printf("\n3. Ищем несуществующее слово 'xyz': ");
    result = btreeSearch(tree, "xyz");
    if (result && result->size == 0) {
        printf("%sПРАВИЛЬНО не найдено%s\n", GREEN, RESET);
    } else {
        printf("%sОШИБКА: должно быть пусто%s\n", RED, RESET);
    }
    vectorFree(result);
    
    printf("\n%s✅ Тест 1 пройден!%s\n", GREEN, RESET);
    freeBTree(tree);
}

// ТЕСТ 2: Дубликаты ключей
void test2_duplicates() {
    printf("\n%s════════════════════════════════════════════%s\n", BOLD, RESET);
    printf("%s📝 ТЕСТ 2: Дубликаты ключей (постинг-листы)%s\n", BOLD, RESET);
    printf("%s════════════════════════════════════════════%s\n\n", BOLD, RESET);
    
    BTree* tree = createBTree();
    
    printf("Вставляем 3 документа с одинаковым словом 'database':\n");
    btreeInsert(tree, "database", 1, "SQL Tutorial");
    printf("  ✓ doc1: SQL Tutorial\n");
    btreeInsert(tree, "database", 2, "NoSQL Guide");
    printf("  ✓ doc2: NoSQL Guide\n");
    btreeInsert(tree, "database", 3, "Database Design");
    printf("  ✓ doc3: Database Design\n");
    
    printf("\nИщем 'database':\n");
    Vector* result = btreeSearch(tree, "database");
    if (result && result->size == 3) {
        printf("  %sНАЙДЕНО %d документа!%s\n", GREEN, result->size, RESET);
        for (int i = 0; i < result->size; i++) {
            values_from_keys* v = (values_from_keys*)getVectorItem(result, i);
            printf("    %d. Документ %d: %s\n", i+1, v->doc_id, v->title);
        }
    } else {
        printf("  %sОШИБКА: найдено %d документов (должно быть 3)%s\n", 
               RED, result ? result->size : 0, RESET);
    }
    vectorFree(result);
    
    printf("\n%s✅ Тест 2 пройден!%s\n", GREEN, RESET);
    freeBTree(tree);
}

// ТЕСТ 3: Визуализация структуры дерева
void test3_visualization() {
    printf("\n%s════════════════════════════════════════════%s\n", BOLD, RESET);
    printf("%s🌳 ТЕСТ 3: Визуализация B-дерева%s\n", BOLD, RESET);
    printf("%s════════════════════════════════════════════%s\n\n", BOLD, RESET);
    
    BTree* tree = createBTree();
    
    printf("Вставляем 10 слов в разном порядке:\n");
    const char* words[] = {"cat", "apple", "dog", "zebra", "banana", 
                          "mouse", "elephant", "fox", "lion", "tiger"};
    
    for (int i = 0; i < 10; i++) {
        printf("  + %s\n", words[i]);
        btreeInsert(tree, words[i], i+1, words[i]);
    }
    
    printf("\n%s📊 СТРУКТУРА ДЕРЕВА:%s\n\n", BOLD, CYAN);
    printTree(tree->root, 0, "");
    
    printf("\n%s📈 Статистика:%s\n", BOLD, RESET);
    printf("  • Всего узлов: %d\n", countNodes(tree->root));
    printf("  • Всего ключей: %d\n", countAllKeys(tree->root));
    printf("  • Корень содержит: %d ключей\n", tree->root->key_count);
    
    printf("\n%s✅ Тест 3 пройден!%s\n", GREEN, RESET);
    freeBTree(tree);
}

// ТЕСТ 4: Поиск и производительность
void test4_search_performance() {
    printf("\n%s════════════════════════════════════════════%s\n", BOLD, RESET);
    printf("%s⚡ ТЕСТ 4: Поиск и производительность%s\n", BOLD, RESET);
    printf("%s════════════════════════════════════════════%s\n\n", BOLD, RESET);
    
    BTree* tree = createBTree();
    
    printf("Заполняем дерево 5000 случайными ключами...\n");
    char keys[5000][20];
    
    clock_t start = clock();
    for (int i = 0; i < 5000; i++) {
        sprintf(keys[i], "word_%d", i);
        btreeInsert(tree, keys[i], i, "Document");
    }
    clock_t end = clock();
    double insert_time = (double)(end - start) / CLOCKS_PER_SEC;
    
    printf("  %sГотово!%s Вставка: %.3f сек (%.0f вставок/сек)\n", 
           GREEN, RESET, insert_time, 5000.0/insert_time);
    
    printf("\nПоиск 100 случайных ключей...\n");
    int found = 0;
    start = clock();
    for (int i = 0; i < 100; i++) {
        int idx = rand() % 5000;
        Vector* res = btreeSearch(tree, keys[idx]);
        if (res && res->size > 0) found++;
        vectorFree(res);
    }
    end = clock();
    double search_time = (double)(end - start) / CLOCKS_PER_SEC;
    
    printf("  %sНайдено: %d/100 (%.1f%%)%s\n", 
           found == 100 ? GREEN : YELLOW, found, (float)found, RESET);
    printf("  Время поиска: %.4f сек (%.0f поисков/сек)\n", 
           search_time, 100.0/search_time);
    
    printf("\n%s✅ Тест 4 пройден!%s\n", GREEN, RESET);
    freeBTree(tree);
}

// ТЕСТ 5: Реальный сценарий (поиск документов)
void test5_real_scenario() {
    printf("\n%s════════════════════════════════════════════%s\n", BOLD, RESET);
    printf("%s🔍 ТЕСТ 5: Реальный сценарий поиска%s\n", BOLD, RESET);
    printf("%s════════════════════════════════════════════%s\n\n", BOLD, RESET);
    
    BTree* index = createBTree();
    
    // Создаем документы
    printf("Индексируем 5 документов:\n\n");
    
    printf("  %sДокумент 1:%s 'Введение в Python'\n", BOLD, RESET);
    btreeInsert(index, "python", 1, "Введение в Python");
    btreeInsert(index, "программирование", 1, "Введение в Python");
    btreeInsert(index, "основы", 1, "Введение в Python");
    
    printf("  %sДокумент 2:%s 'Базы данных SQL'\n", BOLD, RESET);
    btreeInsert(index, "sql", 2, "Базы данных SQL");
    btreeInsert(index, "базы", 2, "Базы данных SQL");
    btreeInsert(index, "данных", 2, "Базы данных SQL");
    
    printf("  %sДокумент 3:%s 'Машинное обучение'\n", BOLD, RESET);
    btreeInsert(index, "машинное", 3, "Машинное обучение");
    btreeInsert(index, "обучение", 3, "Машинное обучение");
    btreeInsert(index, "python", 3, "Машинное обучение");
    
    printf("  %sДокумент 4:%s 'Веб-разработка'\n", BOLD, RESET);
    btreeInsert(index, "web", 4, "Веб-разработка");
    btreeInsert(index, "html", 4, "Веб-разработка");
    btreeInsert(index, "css", 4, "Веб-разработка");
    
    printf("  %sДокумент 5:%s 'Алгоритмы'\n", BOLD, RESET);
    btreeInsert(index, "алгоритмы", 5, "Алгоритмы");
    btreeInsert(index, "структуры", 5, "Алгоритмы");
    btreeInsert(index, "данных", 5, "Алгоритмы");
    
    printf("\n%sПоисковые запросы:%s\n", BOLD, RESET);
    
    // Поиск 1
    printf("\n  Запрос 'python': ");
    Vector* res = btreeSearch(index, "python");
    if (res && res->size > 0) {
        printf("%sНАЙДЕНО %d документа(ов)%s\n", GREEN, res->size, RESET);
        for (int i = 0; i < res->size; i++) {
            values_from_keys* v = (values_from_keys*)getVectorItem(res, i);
            printf("    → Документ %d: %s\n", v->doc_id, v->title);
        }
    }
    vectorFree(res);
    
    // Поиск 2
    printf("\n  Запрос 'данных': ");
    res = btreeSearch(index, "данных");
    if (res && res->size > 0) {
        printf("%sНАЙДЕНО %d документа(ов)%s\n", GREEN, res->size, RESET);
        for (int i = 0; i < res->size; i++) {
            values_from_keys* v = (values_from_keys*)getVectorItem(res, i);
            printf("    → Документ %d: %s\n", v->doc_id, v->title);
        }
    }
    vectorFree(res);
    
    // Поиск 3
    printf("\n  Запрос 'несуществующее_слово': ");
    res = btreeSearch(index, "несуществующее_слово");
    if (res && res->size == 0) {
        printf("%sНЕ НАЙДЕНО (правильно)%s\n", GREEN, RESET);
    }
    vectorFree(res);
    
    printf("\n%s✅ Тест 5 пройден!%s\n", GREEN, RESET);
    freeBTree(index);
}

// ТЕСТ 6: Стресс-тест с дубликатами
void test6_stress_duplicates() {
    printf("\n%s════════════════════════════════════════════%s\n", BOLD, RESET);
    printf("%s💪 ТЕСТ 6: Стресс-тест (1000 дубликатов)%s\n", BOLD, RESET);
    printf("%s════════════════════════════════════════════%s\n\n", BOLD, RESET);
    
    BTree* tree = createBTree();
    
    printf("Добавляем 1000 документов с ключом 'popular':\n");
    
    clock_t start = clock();
    for (int i = 0; i < 1000; i++) {
        btreeInsert(tree, "popular", i, "Popular Document");
        if ((i + 1) % 200 == 0) {
            printf("  Прогресс: %d/1000\n", i + 1);
        }
    }
    clock_t end = clock();
    double time = (double)(end - start) / CLOCKS_PER_SEC;
    
    printf("\nПроверяем результат:\n");
    Vector* res = btreeSearch(tree, "popular");
    if (res && res->size == 1000) {
        printf("  %s✓ Найдено %d документов!%s\n", GREEN, res->size, RESET);
        printf("  ✓ Время вставки: %.2f сек\n", time);
        printf("  ✓ Скорость: %.0f вставок/сек\n", 1000.0/time);
    } else {
        printf("  %s✗ ОШИБКА: найдено %d (должно быть 1000)%s\n", 
               RED, res ? res->size : 0, RESET);
    }
    vectorFree(res);
    
    printf("\n%s✅ Тест 6 пройден!%s\n", GREEN, RESET);
    freeBTree(tree);
}

// Функция для вывода красивого меню
void printMenu() {
    printf("\n%s╔════════════════════════════════════════════════════╗%s\n", BOLD, RESET);
    printf("%s║       B-ДЕРЕВО - ИНТЕРАКТИВНОЕ ТЕСТИРОВАНИЕ       ║%s\n", BOLD, RESET);
    printf("%s╠════════════════════════════════════════════════════╣%s\n", BOLD, RESET);
    printf("%s║  1. Базовые операции                               ║%s\n", RESET);
    printf("%s║  2. Дубликаты ключей (постинг-листы)               ║%s\n", RESET);
    printf("%s║  3. Визуализация дерева                            ║%s\n", RESET);
    printf("%s║  4. Производительность поиска                      ║%s\n", RESET);
    printf("%s║  5. Реальный сценарий (поиск документов)          ║%s\n", RESET);
    printf("%s║  6. Стресс-тест (1000 дубликатов)                 ║%s\n", RESET);
    printf("%s║  7. ЗАПУСТИТЬ ВСЕ ТЕСТЫ                           ║%s\n", BOLD);
    printf("%s║  0. Выход                                          ║%s\n", RESET);
    printf("%s╚════════════════════════════════════════════════════╝%s\n", BOLD, RESET);
    printf("\n%sВыберите тест: %s", BOLD, RESET);
}

int main() {
    srand(time(NULL));
    
    int choice;
    
    // Установка кодировки для Windows
    #ifdef _WIN32
        system("chcp 65001 > nul");
    #endif
    
    do {
        printMenu();
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
                test1_basic();
                break;
            case 2:
                test2_duplicates();
                break;
            case 3:
                test3_visualization();
                break;
            case 4:
                test4_search_performance();
                break;
            case 5:
                test5_real_scenario();
                break;
            case 6:
                test6_stress_duplicates();
                break;
            case 7:
                printf("\n%s🚀 ЗАПУСК ВСЕХ ТЕСТОВ...%s\n", BOLD, RESET);
                test1_basic();
                test2_duplicates();
                test3_visualization();
                test4_search_performance();
                test5_real_scenario();
                test6_stress_duplicates();
                printf("\n%s🎉 ВСЕ ТЕСТЫ ЗАВЕРШЕНЫ!%s\n", BOLD, GREEN, RESET);
                break;
            case 0:
                printf("\n%s👋 До свидания!%s\n\n", BOLD, RESET);
                break;
            default:
                printf("\n%s❌ Неверный выбор! Попробуйте снова.%s\n", RED, RESET);
        }
        
        if (choice != 0 && choice <= 7) {
            printf("\n%sНажмите Enter для продолжения...%s", BOLD, RESET);
            getchar();
            getchar();
        }
        
    } while (choice != 0);
    
    return 0;
}