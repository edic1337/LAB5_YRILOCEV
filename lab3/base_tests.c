#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "list/generic.h"
#include "vector/generic.h"
#include "comparators.h"
#include "base_tasks.h"
//#include <windows.h>

int main() {
    //SetConsoleOutputCP(CP_UTF8);
    printf("тест для студентов\n");
    GenericList* student_list = createList(sizeof(Student));
    Student s1 = {"Иванов Иван Иванович", 4.75};
    Student s2 = {"Анитпов Антон Владимирович", 3.55};
    Student s3 = {"Макеев Сергей Петрович", 4.68};
    Student s4 = {"Романенко Юрий Михайлович", 2.75};
    appendItem(student_list, &s1);
    appendItem(student_list, &s2);
    appendItem(student_list, &s3);
    appendItem(student_list, &s4);
    Student *bestStudent = findMaxStudent(student_list);
    if (bestStudent != NULL) {
        printf("Студент с максимальным баллом: %s, средний балл: %.2f\n", 
               bestStudent->name, bestStudent->avg);
    }
    freeList(student_list);
    
    printf("\n");
    
    // int
    printf("тест для int\n");
    int numbers[] = {5, 2, 9, 1, 7};
    Vector *intVector = createVector(sizeof(int));
    for (int i = 0; i < 5; i++) {
        appendVectorItem(intVector, &numbers[i]);
    }
    int *intMax = findMaxVector(intVector, intGreater);
    if (intMax != NULL) {
        printf("Максимум: %d\n", *intMax);
        free(intMax);
    }
    vectorFree(intVector);
    
    // float
    printf("тест для floatn");
    float floatNumbers[] = {3.14f, 2.71f, 9.99f, 1.05f, 7.77f};
    Vector *floatVector = createVector(sizeof(float));
    for (int i = 0; i < 5; i++) {
        appendVectorItem(floatVector, &floatNumbers[i]);
    }
    float *floatMax = findMaxVector(floatVector, floatGreater);  
    if (floatMax != NULL) {
        printf("Максимум: %.2f\n", *floatMax);
        free(floatMax);
    }
    vectorFree(floatVector);
    
    // тест для long int в векторе
    printf("тест для long int\n");
    long longNumbers[] = {500L, 200L, 900L, 100L, 700L};
    Vector *longVector = createVector(sizeof(long));
    for (int i = 0; i < 5; i++) {
        appendVectorItem(longVector, &longNumbers[i]);
    }
    long *longMax = findMaxVector(longVector, longGreater);  
    if (longMax != NULL) {
        printf("Максимум: %ld\n", *longMax);
        free(longMax);
    }
    vectorFree(longVector);
    
    // тест удаления дубликатов из вектора
    printf("тест удаления дубликатов из вектора\n");
    int dupNumbers[] = {5, 2, 5, 1, 2, 9};
    Vector *dupVector = createVector(sizeof(int));
    for (int i = 0; i < 6; i++) {
        appendVectorItem(dupVector, &dupNumbers[i]);
    }
    printf("до удаления дубликатов: ");
    for (size_t i = 0; i < dupVector->size; i++) {
        int *item = getVectorItem(dupVector, i);
        printf("%d ", *item);
    }
    printf("\n");
    
    removeDuplicatesVector(dupVector, intEquals);
    
    printf("после удаления дубликатов: ");
    for (size_t i = 0; i < dupVector->size; i++) {
        int *item = getVectorItem(dupVector, i);
        printf("%d ", *item);
    }
    printf("\n");
    vectorFree(dupVector);
    
    return 0;
}