#include "tasks.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../lab3/vector/generic.h"

char *my_strdup(const char *src) { // дублирование строки, чтобы в хэш таблице не лежало последнее значение
    char *dst = malloc(strlen(src) + 1);
    if (dst) strcpy(dst, src);
    return dst; // возвращет адрес ячеек с дубликатом
}

void process_file(const char *filename) {
    printf("read %s\n", filename);
    FILE *fp = fopen(filename, "r"); // прочитали строки
    if (!fp) {
        return;
    }
    HashTable *table = createHashTable(sizeof(char *), sizeof(int)); // ключ строка значение число
    Vector *string_storage = createVector(sizeof(char *)); // вектор с строками
    char buffer[4171]; // читаем сюда строки
    while (fgets(buffer, sizeof(buffer), fp)) {
        buffer[strcspn(buffer, "\n")] = 0; // удаляем символ переноса строки \n в конце
        char *temp_str = buffer; 
        int *count_ptr = (int *)getItemHashTable(table, &temp_str, HashStringPtr, CmpStringPtr); // ищем строку
        if (count_ptr != NULL) { // ключ уже есть.
            (*count_ptr)++;
        } else { // ключа еще нет
            char *permanent_str = my_strdup(buffer);
            appendVectorItem(string_storage, &permanent_str); // сохраняем указатель чтобы потом free
            int init_val = 1;
            setItemHashTable(table, &permanent_str, &init_val, HashStringPtr, CmpStringPtr); // в таблицу со значением 1
        }
    }
    unsigned long collisions = getCollisionCount(table, HashStringPtr); // статистика 
    printf("statistic\n");
    printf("Capacity:   %lu\n",(unsigned long)table -> capacity);
    printf("Collisions: %lu\n", collisions);
    fclose(fp); // не забываем почистить
    freeHashTable(table);
    for (size_t i = 0; i < string_storage -> size; i++) {
        char **s = (char **)getVectorItem(string_storage, i);
        free(*s); // освобождаем саму строку
    }
    vectorFree(string_storage);
}

int main() {
    process_file("data/good_cases.csv");
    process_file("data/bad_cases.csv");
    return 0;
}
