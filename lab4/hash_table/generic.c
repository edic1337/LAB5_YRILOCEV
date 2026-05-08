#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "../vector/generic.h"
#include "generic.h"
#include <math.h>

#define NUMB_HASH 2654435761U
#define HASH_START 5381

int HashInt(const void *key)
{
    if (!key) return 0;
    unsigned int hash = (*(int*)key) * NUMB_HASH;
    return (int)(hash >> 16) & 0x7FFFFFFF;
}

int HashString(const void *key)
{
    if (!key) {
        return 0;
    }
    const char *str = (const char *)key;
    if (!str) { 
        return 0; // если сама строка NULL
    }
    unsigned long hash = HASH_START;
    int c;
    while ((c = *str++)) {
        // hash * 33 + c
        hash = ((hash << 5) + hash) + c; // битовый сдвиг влево = (*2)
    }
    return (int)(hash & 0x7FFFFFFF); // & 0x7FFFFFFF гарантирует, что число будет положительным
}

// компаратор для ключей типа long long
int CmpLongLong(const void *a, const void *b)
{
    if (a == NULL || b == NULL) {
        return 0;
    }

    const long long *pointer_a = (const long long *)a;
    const long long *pointer_b = (const long long *)b;
    long long value_a = *pointer_a;
    long long value_b = *pointer_b;

    if (value_a == value_b) {
        return 1;
    } else {
        return 0;
    }
}

// хэш-функция для long long
int HashLongLong(const void *key)
{
    if (key == NULL) {
        return 0;
    }

    const unsigned long long *pointer_id = (const unsigned long long *)key;
    unsigned long long id = *pointer_id;

    unsigned long long shifted_id = id >> 32;

    unsigned int mixed_bits = (unsigned int)(id ^ shifted_id);

    unsigned int hash = mixed_bits * NUMB_HASH;

    int positive_hash = (int)(hash & 0x7FFFFFFF);

    return positive_hash;
}

HashTable *createHashTable(size_t key_size, size_t val_size)
{
    HashTable *tab = (HashTable* )malloc(sizeof(HashTable)); // выделили память под таблицу и сделали правильный тип указателя
    if (!tab) {
        return NULL;
    }
    tab -> key_size = key_size;
    tab -> val_size = val_size;
    tab -> capacity = TABLE_MIN_SIZE;
    tab -> size = 0;
    size_t item_size = sizeof(uint8_t) + key_size + val_size; // размер данных + ключа + байт под флаг
    tab -> values = createVector(item_size);
    if (!tab -> values) {
        free(tab);
        return NULL;
    }
    // заполняем вектор нулями
    // создаем массив с нулями и добавляем его capacity раз
    void *empty_slot = calloc(1, item_size); // calloc сам занулит память
    if (!empty_slot) {
        vectorFree(tab->values);
        free(tab);
        return NULL;
    }
    for (size_t i = 0; i < tab -> capacity; i++) {
        appendVectorItem(tab -> values, empty_slot); // теперь vector -> size равен vector -> capacity, и мы можем делать get/set по любому индексу
    }
    free(empty_slot);
    return tab;
}

void setItemHashTable(HashTable *table, void *key, void *data, HashFunc hash, CmpFunc cmp)
{   
    if (!table || !key || !data || !hash || !cmp) {
       return; 
    } 
    if (table->size * 2 >= table -> capacity) {
        rehashHashTable(table, hash, cmp);
    }
    unsigned int h = (unsigned int)hash(key);
    size_t idx = h % table -> capacity;
    size_t first_deleted = SIZE_MAX;
    for (size_t i = 0; i < table -> capacity; i++) { // линейное пробирование 
        size_t current = (idx + i) % table -> capacity;
        void *slot = getVectorItem(table -> values, current);
        uint8_t *flag = (uint8_t *)slot;
        if (*flag == SLOT_OCCUPIED) {
            void *slot_key = flag + 1;
            if (cmp(slot_key, key)) { // совпали ключи обновляем значение
                void *slot_val = flag + 1 + table -> key_size;
                memcpy(slot_val, data, table -> val_size);
                return;
            }
        }
        else if (*flag == SLOT_EMPTY) {
            size_t insert_at;
            if (first_deleted != SIZE_MAX) {
                insert_at = first_deleted;  // если нашли удаленный слот вставляем туда
            } else {
                insert_at = current;        // иначе вставляем в текущий пустой слот
            }
            if (insert_at != current) {
                slot = getVectorItem(table -> values, insert_at);
                flag = (uint8_t *)slot;
            }
            *flag = SLOT_OCCUPIED;
            void *slot_key = flag + 1;
            void *slot_val = flag + 1 + table -> key_size;
            memcpy(slot_key, key, table -> key_size);
            memcpy(slot_val, data, table -> val_size);
            table->size++;
            return;
        }
        else if (*flag == SLOT_DELETED) {
            if (first_deleted == SIZE_MAX) {
                first_deleted = current;
            }
        }
    }
    if (first_deleted != SIZE_MAX) {
        void *slot = getVectorItem(table -> values, first_deleted);
        uint8_t *flag = (uint8_t *)slot;
        *flag = SLOT_OCCUPIED;
        void *slot_key = flag + 1;
        void *slot_val = flag + 1 + table -> key_size;
        memcpy(slot_key, key, table -> key_size);
        memcpy(slot_val, data, table -> val_size);
        table->size++;
    }
}

void rehashHashTable(HashTable *table, HashFunc hash, CmpFunc cmp)
{
    if (!table || !hash) {
        return;
    } 
    Vector *old_values = table -> values;
    size_t old_capacity = table -> capacity;
    (void)cmp;
    table -> capacity *= 2; // увеличили таблицу
    table -> size = 0; // будем все копировать заново
    size_t slot_size = sizeof(uint8_t) + table -> key_size + table -> val_size;
    table -> values = createVector(slot_size);
    void *empty_slot = calloc(1, slot_size);
    for (size_t i = 0; i < table -> capacity; i++) { // заполняем новый вектор пустыми слотами
        appendVectorItem(table -> values, empty_slot);
    }
    free(empty_slot); // не забываем почистить 
    for (size_t i = 0; i < old_capacity; i++) { // переносим все значения 
        void *old_slot_ptr = getVectorItem(old_values, i);
        uint8_t *flag_ptr = (uint8_t *)old_slot_ptr;
        if (*flag_ptr == SLOT_OCCUPIED) {
            void *key_ptr = (void *)(flag_ptr + 1);
            void *val_ptr = (void *)(flag_ptr + 1 + table -> key_size);
            unsigned int h = (unsigned int)hash(key_ptr);
            size_t ind = h % table -> capacity;
            for (size_t j = 0; j < table -> capacity; j++) {
                void *new_slot = getVectorItem(table -> values, ind);
                uint8_t *new_flag = (uint8_t *)new_slot;
                if (*new_flag == SLOT_EMPTY) {
                    // Копируем данные
                    *new_flag = SLOT_OCCUPIED;
                    memcpy((uint8_t*)new_flag + 1, key_ptr, table -> key_size);
                    memcpy((uint8_t*)new_flag + 1 + table -> key_size, val_ptr, table -> val_size);
                    table -> size++;
                    break; 
                }
                ind = (ind + 1) % table -> capacity;
            }
        }
    }
    vectorFree(old_values); // удаляем старый вектор
}

void *getItemHashTable(HashTable *table, void *key, HashFunc hash, CmpFunc cmp)
{
    if (!table || !key || !hash || !cmp) {
        return NULL;
    }
    unsigned int raw_hash = (unsigned int)hash(key);
    size_t index = raw_hash % table->capacity;

    for (size_t i = 0; i < table->capacity; i++) {
        size_t current_index = (index + i) % table->capacity;
        void *slot_ptr = getVectorItem(table->values, current_index);
        uint8_t *flag_ptr = (uint8_t *)slot_ptr;

        if (*flag_ptr == SLOT_EMPTY) {
            return NULL;
        }

        if (*flag_ptr == SLOT_OCCUPIED) {
            void *key_ptr = flag_ptr + 1;
            if (cmp(key_ptr, key)) {
                return flag_ptr + 1 + table->key_size;
            }
        }
    }

    return NULL;
}

void *popItemHashTable(HashTable *table, void *key, HashFunc hash, CmpFunc cmp)
{
    if (!table || !key || !hash || !cmp) return NULL;
    unsigned int raw_hash = (unsigned int)hash(key);
    size_t index = raw_hash % table -> capacity;
    for (size_t i = 0; i < table -> capacity; i++) { // то же самое линейное пробирование 
        size_t current_index = (index + i) % table -> capacity;
        void *slot_ptr = getVectorItem(table->values, current_index);
        uint8_t *flag_ptr = (uint8_t *)slot_ptr;
        void *key_ptr = (void *)(flag_ptr + 1);
        void *values_ptr = (void *)(flag_ptr + 1 + table -> key_size);
        if (*flag_ptr == SLOT_EMPTY) {
            return NULL;
        }
        if (*flag_ptr == SLOT_OCCUPIED) {
            if (cmp(key_ptr, key)) { // нашли кого удалять
                void *result = malloc(table -> val_size); // память под копию
                if (result) {
                    memcpy(result, values_ptr, table -> val_size); // копируем значение
                }
                *flag_ptr = SLOT_DELETED; // поменяли флаг
                table -> size--;
                return result;
            }
        }
    }
    return NULL;
}

unsigned long int getCollisionCount(HashTable *table, HashFunc hash)
{
    if (!table || !hash) {
        return 0;
    }
    unsigned long int collisions = 0;
    for (size_t i =0; i < table -> capacity; i++) {
        void *slot_ptr = getVectorItem(table -> values, i);
        uint8_t *flag_ptr = (uint8_t *)slot_ptr;
        if (*flag_ptr == SLOT_OCCUPIED) {
            void *key_ptr = (void *)(flag_ptr + 1);
            unsigned int raw_hash = (unsigned int)hash(key_ptr); // хэщ элемента без пробирования
            size_t ideal_index = raw_hash % table -> capacity;
            if (i != ideal_index) { // хэш не совпал с индексом элемента - была коллизия
                collisions++;
            }
        }   
    }
    return collisions;
}

void freeHashTable(HashTable *table)
{
    if (table) {
        if (table -> values) {
            vectorFree(table -> values);
        }
        free(table);
    }
}