#include "tasks.h"
#include "hash_table/generic.h"
#include "../lab3/vector/generic.h"
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <math.h>

#define HASH_START 5381
#define A_GOLDEN  (sqrt(5.0) - 1) / 2 

int HashStringPtr(const void *key)
{
    if (!key) {
        return 0;
    }
    char **str_ptr = (char **)key;
    char *str = *str_ptr;
    unsigned long hash = 5381;
    int c;
    if (!str) {
        return 0;
    }
    while ((c = *str++)) {
        // hash * 33 + c
        hash = ((hash << 5) + hash) + c; 
    }
    return (int)(hash & 0x7FFFFFFF); // & 0x7FFFFFFF убирает знаковый бит, чтобы не было отрицательных чисел
}

int CmpStringPtr(const void *a, const void *b)
{
    const char *s1 = *(const char **)a;
    const char *s2 = *(const char **)b;
    if (s1 == s2) return 1; 
    if (!s1 || !s2) return 0; // защита от NULL
    return strcmp(s1, s2) == 0;
}


Vector *removeDuplicates(Vector *strings)
{
    if (!strings) return NULL;
    Vector *result = createVector(sizeof(char*)); // создаем вектор результата. хранит char*
    if (!result) {
    return NULL;
    }
    HashTable *seen = createHashTable(sizeof(char*), sizeof(char));
    if (!seen) {
        vectorFree(result);
        return NULL;
    }
    int dummy_val = 1;
    for (size_t i = 0; i < strings->size; i++) {
        void *elem_ptr = getVectorItem(strings, i); // получаем адрес элемента вектора (char**)
        char *str = *(char **)elem_ptr; // достаем саму строку для удобства
        // передаем &str, так как таблица ожидает указатель на ключ
        if (getItemHashTable(seen, &str, HashStringPtr, CmpStringPtr) == NULL) {
            appendVectorItem(result, &str); // если нет — добавляем в результат и в таблицу
            setItemHashTable(seen, &str, &dummy_val, HashStringPtr, CmpStringPtr);
        }
    }
    freeHashTable(seen);
    return result;
}

Vector *encodeStrings(Vector *strings)
{
    Vector *result = createVector(sizeof(int)); // тут результат
    HashTable *map = createHashTable(sizeof(char *), sizeof(int)); // ключ строка значение инт
    int next = 0;
    for (size_t i = 0; i < strings -> size; i++) { //по всем словам вектора 
        int code; // код для строк
        void *item_ptr = getVectorItem(strings, i); // адрес ячейки вектора
        char *current_str = *(char **)item_ptr; // сама строка
        void *found_val = getItemHashTable(map, &current_str, HashStringPtr, CmpStringPtr); // NULL или код
        if (found_val != NULL) {
            code = *(int *)found_val;
        }
        else {
            code = next;
            next++;
            setItemHashTable(map, &current_str, &code, HashStringPtr, CmpStringPtr);
        }
        appendVectorItem(result, &code);
    } 
    freeHashTable(map);
    return result;
}

HashTable *swapKeysValues(HashTable *table, HashFunc hash, CmpFunc cmp)
{
    if (table == NULL) {
        return NULL;
    }
    HashTable *new_table = createHashTable(table->val_size, table->key_size); // поменяли размеры местами
    Vector *vec = table -> values;
    for (size_t i = 0; i < vec->size; i++) {
        void *slot = getVectorItem(vec, i);
        uint8_t flag = *(uint8_t *)slot;
        if (flag == SLOT_OCCUPIED) {
            void *old_key = (uint8_t *)slot + 1;
            void *old_val = (uint8_t *)slot + 1 + table -> key_size;
            setItemHashTable(new_table, old_val, old_key, hash, cmp); // вставили в новую таблицу значения и ключ наоборот
        }
    }
    return new_table;
}