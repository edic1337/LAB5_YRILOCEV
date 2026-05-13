#include "btree.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Функция для визуализации узла дерева (с отступами)
void visualizeNode(BTreeNode* node, int depth, const char* prefix, int is_last) {
    if (!node) return;
    
    // Отступы для текущего уровня
    for (int i = 0; i < depth - 1; i++) {
        printf("│   ");
    }
    
    if (depth > 0) {
        printf("%s", prefix);
        printf("%s", is_last ? "└── " : "├── ");
    }
    
    // Выводим ключи узла
    printf("[");
    for (int i = 0; i < node->key_count; i++) {
        printf("%s", node->keys[i]);
        if (node->postings[i]) {
            printf("(%d)", node->postings[i]->size);
        }
        if (i < node->key_count - 1) printf(", ");
    }
    printf("]");
    
    if (node->is_leaf) {
        printf(" (LEAF)\n");
    } else {
        printf(" (INTERNAL)\n");
    }
    
    // Рекурсивно выводим детей
    if (!node->is_leaf) {
        for (int i = 0; i <= node->key_count; i++) {
            int is_last_child = (i == node->key_count);
            char new_prefix[256];
            char new_prefix2[256];
            
            if (depth == 0) {
                snprintf(new_prefix, sizeof(new_prefix), "%s", "");
                snprintf(new_prefix2, sizeof(new_prefix2), "%s", "");
            } else {
                snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix, is_last ? "    " : "│   ");
                snprintf(new_prefix2, sizeof(new_prefix2), "%s", new_prefix);
            }
            
            visualizeNode(node->children[i], depth + 1, new_prefix2, is_last_child);
        }
    }
}

// Визуализация всего дерева
void visualizeTree(BTree* tree, const char* title) {
    if (!tree || !tree->root) {
        printf("Дерево пустое или не инициализировано\n");
        return;
    }
    
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  %-60s ║\n", title);
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    
    printf("Уникальных ключей: %d\n", tree->unique_key);
    printf("Структура дерева:\n");
    
    if (tree->root->key_count == 0) {
        printf("  [пустое дерево]\n");
    } else {
        visualizeNode(tree->root, 0, "", 1);
    }
    printf("\n");
}

// Функция для демонстрации поиска
void searchAndShow(BTree* tree, const char* key) {
    printf("\n🔍 Поиск ключа: \"%s\"\n", key);
    
    Vector* result = btreeSearch(tree, key);
    
    if (!result) {
        printf("   ❌ Ошибка: btreeSearch вернул NULL\n");
        return;
    }
    
    if (result->size == 0) {
        printf("   ❌ Ключ НЕ НАЙДЕН\n");
    } else {
        printf("   ✅ Ключ НАЙДЕН!\n");
        printf("   📊 Количество вхождений: %d\n", result->size);
        
        // Пытаемся вывести содержимое (если есть доступ к данным)
        printf("   📄 Содержимое постинг-листа:\n");
        for (int i = 0; i < result->size && i < 10; i++) {
            // ВАЖНО: здесь нужен доступ к элементам вектора
            // Если есть функция vectorGet(), используйте её
            // Пока выводим только размер
            printf("      [%d] запись #%d\n", i, i+1);
        }
    }
    
    vectorFree(result);
}

// Тест 1: Поиск в пустом дереве
void test_empty_tree() {
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("ТЕСТ 1: Поиск в пустом дереве\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    BTree* tree = createBTree();
    visualizeTree(tree, "Пустое дерево");
    
    searchAndShow(tree, "anything");
    searchAndShow(tree, "test");
    searchAndShow(tree, "ключ");
    
    freeBTree(tree);
}

// Тест 2: Поиск после вставки одного элемента
void test_single_element() {
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("ТЕСТ 2: Поиск после вставки одного элемента\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    BTree* tree = createBTree();
    
    printf("\n➡️ Вставляем ключ \"apple\" с документом 1\n");
    btreeInsert(tree, "apple", 1, "Apple Document");
    
    visualizeTree(tree, "Дерево с одним элементом");
    
    searchAndShow(tree, "apple");
    searchAndShow(tree, "banana");
    searchAndShow(tree, "APPLE");
    
    freeBTree(tree);
}

// Тест 3: Поиск в дереве с несколькими элементами (без разделения)
void test_several_elements() {
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("ТЕСТ 3: Поиск в дереве с несколькими элементами\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    BTree* tree = createBTree();
    
    printf("\n➡️ Вставляем ключи:\n");
    const char* keys[] = {"banana", "apple", "cherry", "date"};
    int n = 4;
    
    for (int i = 0; i < n; i++) {
        printf("   - \"%s\" (документ %d)\n", keys[i], i+1);
        btreeInsert(tree, keys[i], i+1, "Document");
    }
    
    visualizeTree(tree, "Дерево с несколькими элементами");
    
    printf("\n📋 Поиск существующих ключей:\n");
    for (int i = 0; i < n; i++) {
        searchAndShow(tree, keys[i]);
    }
    
    printf("\n📋 Поиск несуществующих ключей:\n");
    searchAndShow(tree, "grape");
    searchAndShow(tree, "aaaa");
    searchAndShow(tree, "zzzz");
    
    freeBTree(tree);
}

// Тест 4: Поиск в дереве с дубликатами
void test_duplicates() {
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("ТЕСТ 4: Поиск ключа с дубликатами\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    BTree* tree = createBTree();
    
    printf("\n➡️ Вставляем дубликаты ключа \"test\":\n");
    btreeInsert(tree, "test", 1, "First document");
    btreeInsert(tree, "test", 2, "Second document");
    btreeInsert(tree, "test", 3, "Third document");
    btreeInsert(tree, "alpha", 4, "Alpha document");
    btreeInsert(tree, "zeta", 5, "Zeta document");
    
    visualizeTree(tree, "Дерево с дубликатами");
    
    searchAndShow(tree, "test");
    searchAndShow(tree, "alpha");
    searchAndShow(tree, "zeta");
    searchAndShow(tree, "beta");
    
    freeBTree(tree);
}

// Тест 5: Поиск после разделения узла
void test_after_split() {
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("ТЕСТ 5: Поиск после разделения корня (BTREE_T=%d, MAX_KEYS=%d)\n", 
           BTREE_T, BTREE_MAX_KEYS);
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    BTree* tree = createBTree();
    
    printf("\n➡️ Вставляем ключи для вызова разделения:\n");
    // Вставляем на 1 больше максимального
    int num_keys = BTREE_MAX_KEYS + 1;
    
    for (int i = 0; i < num_keys; i++) {
        char key[20];
        sprintf(key, "k%d", i * 10); // k0, k10, k20, k30, k40, k50
        printf("   - \"%s\"\n", key);
        btreeInsert(tree, key, i, "Document");
        
        // Показываем дерево после каждой вставки, вызывающей изменения
        if (i == BTREE_MAX_KEYS) {
            printf("\n   ⚠️ После вставки %d-го ключа произошло разделение!\n", i+1);
            visualizeTree(tree, "Дерево после разделения");
        }
    }
    
    visualizeTree(tree, "Финальное дерево после разделения");
    
    printf("\n🔍 Проверяем поиск всех ключей:\n");
    for (int i = 0; i < num_keys; i++) {
        char key[20];
        sprintf(key, "k%d", i * 10);
        searchAndShow(tree, key);
    }
    
    printf("\n🔍 Проверяем поиск несуществующих ключей:\n");
    searchAndShow(tree, "k5");
    searchAndShow(tree, "k15");
    searchAndShow(tree, "k55");
    
    freeBTree(tree);
}

// Тест 6: Глубокий поиск в большом дереве
void test_deep_search() {
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("ТЕСТ 6: Глубокий поиск в большом дереве\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    BTree* tree = createBTree();
    
    printf("\n➡️ Вставляем 50 ключей...\n");
    for (int i = 0; i < 50; i++) {
        char key[30];
        sprintf(key, "word_%03d", i);
        btreeInsert(tree, key, i, "Document");
        
        if ((i + 1) % 10 == 0) {
            printf("   Вставлено %d ключей...\n", i + 1);
        }
    }
    
    visualizeTree(tree, "Большое дерево (50 ключей)");
    
    printf("\n🔍 Поиск ключей в начале:\n");
    searchAndShow(tree, "word_000");
    searchAndShow(tree, "word_005");
    
    printf("\n🔍 Поиск ключей в середине:\n");
    searchAndShow(tree, "word_025");
    searchAndShow(tree, "word_030");
    
    printf("\n🔍 Поиск ключей в конце:\n");
    searchAndShow(tree, "word_045");
    searchAndShow(tree, "word_049");
    
    printf("\n🔍 Поиск несуществующих ключей (бинарный поиск по границам):\n");
    searchAndShow(tree, "word_001"); // существует
    searchAndShow(tree, "word_002"); // существует
    searchAndShow(tree, "word_999"); // не существует
    searchAndShow(tree, "aaa");
    searchAndShow(tree, "zzz");
    
    freeBTree(tree);
}

// Тест 7: Сравнение производительности поиска
void test_search_performance() {
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("ТЕСТ 7: Сравнение поиска в разных местах дерева\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    BTree* tree = createBTree();
    
    // Вставляем ключи в отсортированном порядке
    printf("\n➡️ Создаем дерево с 100 ключами...\n");
    for (int i = 0; i < 100; i++) {
        char key[20];
        sprintf(key, "key_%03d", i);
        btreeInsert(tree, key, i, "Document");
    }
    
    visualizeTree(tree, "Дерево с 100 ключами");
    
    printf("\n📊 Статистика дерева:\n");
    printf("   - Уникальных ключей: %d\n", tree->unique_key);
    printf("   - Корень: %s, ключей: %d\n", 
           tree->root->is_leaf ? "лист" : "внутренний узел",
           tree->root->key_count);
    
    printf("\n🔍 Поиск с замером (визуально):\n");
    
    // Поиск существующих ключей
    printf("\n   Существующие ключи:\n");
    searchAndShow(tree, "key_000");
    searchAndShow(tree, "key_050");
    searchAndShow(tree, "key_099");
    
    // Поиск несуществующих ключей (границы)
    printf("\n   Несуществующие ключи (граничные случаи):\n");
    searchAndShow(tree, "key_100"); // после последнего
    searchAndShow(tree, "key_999"); // далеко после
    searchAndShow(tree, "aaa");     // до первого
    searchAndShow(tree, "key_0455"); // между ключами
    
    freeBTree(tree);
}

// Главная функция
int main() {
    printf("\n╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║                ТЕСТИРОВАНИЕ ПОИСКА В B-ДЕРЕВЕ                    ║\n");
    printf("║                     BTREE_T = %d                                 ║\n", BTREE_T);
    printf("║                BTREE_MAX_KEYS = %d                               ║\n", BTREE_MAX_KEYS);
    printf("╚══════════════════════════════════════════════════════════════════╝\n");
    
    test_empty_tree();
    test_single_element();
    test_several_elements();
    test_duplicates();
    test_after_split();
    test_deep_search();
    test_search_performance();
    
    printf("\n╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║                     ТЕСТИРОВАНИЕ ЗАВЕРШЕНО                       ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}