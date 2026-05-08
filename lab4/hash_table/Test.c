#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../../lab3/vector/generic.h"
#include "generic.h"
#include "../tasks.h"

void test_remove_duplicates()
{
    printf("Test: removeDuplicates()\n");

    Vector *vec = createVector(sizeof(char *));
    char *strings[] = {"apple", "banana", "apple", "cherry", "banana", "date"};
    size_t n = sizeof(strings) / sizeof(strings[0]);

    for (size_t i = 0; i < n; i++)
        appendVectorItem(vec, &strings[i]);

    Vector *unique = removeDuplicates(vec);

    printf("Unique strings:\n");
    for (size_t i = 0; i < unique->size; i++)
    {
        char **s = (char **)getVectorItem(unique, i);
        printf("%s\n", *s);
    }

    assert(unique->size == 4);

    const char *expected[] = {"apple", "banana", "cherry", "date"};
    for (size_t i = 0; i < unique->size; i++)
    {
        char **s = (char **)getVectorItem(unique, i);
        int found = 0;
        for (size_t j = 0; j < 4; j++)
        {
            if (strcmp(*s, expected[j]) == 0)
                found = 1;
        }
        assert(found);
    }

    vectorFree(vec);
    vectorFree(unique);

    printf("removeDuplicates() test passed!\n\n");
}

int main() {
    test_remove_duplicates();
    return 0;
}