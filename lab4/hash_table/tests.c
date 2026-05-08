#include "generic.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int strEqualsDirect(const void *a, const void *b) {
    return strcmp((char*)a, (char*)b) == 0;
}

int BadHash(const void *key) {
    return 0; // все ключи попадают в одну ячейку
}

int main() {
    printf("TESTS\n");
    printf("int test: ");
    {
        HashTable *ht = createHashTable(sizeof(int), sizeof(int));
        int k1 = 1, v1 = 100;
        int k2 = 2, v2 = 200;
        setItemHashTable(ht, &k1, &v1, HashInt, intEquals);
        setItemHashTable(ht, &k2, &v2, HashInt, intEquals);
        int *found = getItemHashTable(ht, &k1, HashInt, intEquals);
        assert(found && *found == 100);
        found = getItemHashTable(ht, &k2, HashInt, intEquals);
        assert(found && *found == 200);
        int k3 = 3;  // несуществующий ключ
        assert(getItemHashTable(ht, &k3, HashInt, intEquals) == NULL);   
        freeHashTable(ht);
        printf("OK\n");
    }
    printf("update test: ");
    {
        HashTable *ht = createHashTable(sizeof(int), sizeof(int));
        int k = 5, v = 50;
        setItemHashTable(ht, &k, &v, HashInt, intEquals);
        v = 500; // новое значение
        setItemHashTable(ht, &k, &v, HashInt, intEquals);
        int *found = getItemHashTable(ht, &k, HashInt, intEquals);
        assert(found && *found == 500);
        freeHashTable(ht);
        printf("OK\n");
    }
    printf("string test: ");
    {
        HashTable *ht = createHashTable(10, sizeof(int)); // 10 байт на строку
        char key1[10] = "hello";
        char key2[10] = "world";
        int val1 = 111, val2 = 222;
        setItemHashTable(ht, key1, &val1, HashString, strEqualsDirect);
        setItemHashTable(ht, key2, &val2, HashString, strEqualsDirect);
        int *found = getItemHashTable(ht, key1, HashString, strEqualsDirect);
        assert(found && *found == 111);
        found = getItemHashTable(ht, key2, HashString, strEqualsDirect);
        assert(found && *found == 222);
        freeHashTable(ht);
        printf("OK\n");
    }
    printf("collision test: ");
    {
        HashTable *ht = createHashTable(sizeof(int), sizeof(int));
        int v = 999;
        // все ключи имеют одинаковый хэш
        int k1 = 1, k2 = 2, k3 = 3;
        setItemHashTable(ht, &k1, &v, BadHash, intEquals);
        setItemHashTable(ht, &k2, &v, BadHash, intEquals);
        setItemHashTable(ht, &k3, &v, BadHash, intEquals);
        // все должны найтись
        assert(getItemHashTable(ht, &k1, BadHash, intEquals) != NULL);
        assert(getItemHashTable(ht, &k2, BadHash, intEquals) != NULL);
        assert(getItemHashTable(ht, &k3, BadHash, intEquals) != NULL);
        freeHashTable(ht);
        printf("OK\n");
    }
    printf("delete test: ");
    {
        HashTable *ht = createHashTable(sizeof(int), sizeof(int));
        int k = 7, v = 777;
        setItemHashTable(ht, &k, &v, HashInt, intEquals);
        // удаляем
        void *deleted = popItemHashTable(ht, &k, HashInt, intEquals);
        assert(deleted != NULL);
        free(deleted);
        // проверяем, что удалился
        assert(getItemHashTable(ht, &k, HashInt, intEquals) == NULL);
        freeHashTable(ht);
        printf("OK\n");
    }
    printf("more items test: ");
    {
        HashTable *ht = createHashTable(sizeof(int), sizeof(int));
        for (int i = 0; i < 5; i++) {
            setItemHashTable(ht, &i, &i, HashInt, intEquals);
        }
        for (int i = 0; i < 5; i++) {
            int *found = getItemHashTable(ht, &i, HashInt, intEquals);
            assert(found && *found == i);
        }
        assert(ht -> size == 5);
        freeHashTable(ht);
        printf("OK\n");
    }
    printf("rehash test: ");
    {
        HashTable *ht = createHashTable(sizeof(int), sizeof(int));
        size_t start_cap = ht -> capacity;
        for (int i = 0; i < 15; i++) {
            setItemHashTable(ht, &i, &i, HashInt, intEquals);
        }
        assert(ht -> capacity > start_cap);
        for (int i = 0; i < 15; i++) {
            int *found = getItemHashTable(ht, &i, HashInt, intEquals);
            assert(found && *found == i);
        }
        freeHashTable(ht);
        printf("OK\n");
    }
    printf("\nTESTS PASSED\n");
    return 0;
}
