#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "generic.h"

// Вспомогательная функция для изменения размера
static bool needToResize(Vector *vector, bool *increase)
{
    if ( vector -> size == vector -> capacity) {
        *increase = true;
        return true;
    }
    if ( MIN_SIZE < vector -> size && vector -> size < (vector -> capacity) / 5) {
        *increase = false;
        return true;
    }
    return false;
}

// Определяем увеличивать размер или уменьшать
static int resize(Vector *vector, bool increase)
{
    size_t new_capacity;
    if (increase) {
        new_capacity = vector -> capacity * 2; // если надо увеличить
    
        if (new_capacity == 0) {
            new_capacity = MIN_SIZE;
        }
    } else {
        new_capacity = vector -> capacity / 2; // если можно уменшить
        if (new_capacity < MIN_SIZE) {
            new_capacity = MIN_SIZE;
        }
    }
    void *new_data = malloc(new_capacity * vector -> elem_size); // память под новые данные
    if (new_data == NULL){
        return -1;
    }
    if (vector -> size > 0) {
        memcpy(new_data, vector -> data, vector -> size * vector -> elem_size);
    }
    free(vector -> data); // не забываем почистить 
    vector -> data = new_data;
    vector -> capacity = new_capacity;
    return 0;
}

Vector *createVector(size_t elem_size)
{
    if (elem_size == 0) {
        return NULL;
    }
    Vector *vector = malloc(sizeof(Vector)); // память под характеристики структуры
    if (vector == NULL) {
        return NULL;
    }
    vector -> data = malloc(MIN_SIZE * elem_size); // память под данные
    if (vector -> data == NULL) {
        free(vector);
        return NULL;
    }
    vector -> size = 0;
    vector -> elem_size = elem_size;
    vector -> capacity = MIN_SIZE;
    return vector;
}

int appendVectorItem(Vector *vector, void *el)
{
    if (vector == NULL || el == NULL) { // проверка на корректность 
        return -1;
    }
    bool increase;
    if (needToResize(vector, &increase)) {
        if (resize(vector, increase) != 0) {
            return -1;  // ошибка расширения памяти
        }
    }
    void *destination = (char*)vector -> data + (vector -> size * vector -> elem_size); // переходим в свободную область памяти для записи данных
    memcpy(destination, el, vector -> elem_size);
    vector -> size++;
    return 0;
}

void *getVectorItem(Vector *vector, size_t index)
{
    if (vector == NULL || index >= vector -> size) {
        return NULL;
    }
    void *find_item = (char*)vector -> data + (index * vector -> elem_size); // нашли элемент арифметикой 
    return find_item;
}

int setVectorItem(Vector *vector, size_t index, void *value)
{
    if (vector == NULL || index >= vector -> size || value == NULL)  {
        return -1;
    }
    void *destination = (char*)vector -> data + (vector -> elem_size * index); // нашли элемент арифметикой 
    memcpy(destination, value, vector -> elem_size);
    return 0;
}

void *popVectorItem(Vector *vector, size_t index)
{
    if (vector == NULL || index >= vector -> size) {
        return NULL;
    }
    void *el_to_remove = (char*)vector -> data + (index * vector -> elem_size); // нашли кого удалять
    void *data_copy = malloc(vector -> elem_size); // память под копию
    if (data_copy == NULL) {
        return NULL;
    }
    memcpy(data_copy, el_to_remove, vector->elem_size);
    if (index < vector -> size -1) {
        for (size_t  i = index; i < vector -> size - 1; i++) { // циклом проходим по всем элементам правее удаляемого
            void *right_el = (char*)vector -> data + ((i + 1) * vector -> elem_size);
            void *destination = (char*)vector -> data + (i * vector -> elem_size);
            memcpy(destination, right_el, vector -> elem_size);
        }
    }
    vector -> size--;
    bool increase;
    if (needToResize(vector, &increase)) {
        resize(vector, increase);
    }
    
    return data_copy;
}

long int findVectorItem(Vector *vector, void *value, EqualsFunc cmp)
{
    if ( vector == NULL || value == NULL || cmp == NULL) {
        return -1;
    }
    for (size_t i = 0; i < vector -> size; i++) {
        void *item = (char*)vector -> data + (i * vector -> elem_size);
        if (cmp(item, value)) {
            return (long long)i;
        }
    }
    return -1; // не нашли 
}

int vectorFree(Vector *vector)
{
    if (vector == NULL) {
        return -1;
    }
    if (vector -> data != NULL) { // один malloc на всех, его и очищаем
        free(vector -> data);
    }
    free(vector);
    return 0;
}