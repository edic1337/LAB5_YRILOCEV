#include "base_tasks.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comparators.h"

Student *findMaxStudent(GenericList *list)
{
    if (list == NULL || list->head == NULL) {
        return NULL; // пустой список
    }
    
    // Начинаем с первого элемента
    Node *current = list->head;
    Student *Max_Student = (Student*)current->data;
    
    // Обходим все остальные элементы
    current = current->next;
    while (current != NULL) {
        Student *student = (Student*)current->data;
        if (student->avg > Max_Student->avg) {
            Max_Student = student;
        }
        current = current->next;
    }
    
    return Max_Student;
}

void *findMaxVector(Vector *vector, EqualsFunc cmp)
{
    if (vector == NULL || vector -> size == NULL) {
        return NULL;
    }
    void *max_elem = getVectorItem(vector, 0);
    for (size_t i = 1; i < vector -> size; i++) {
        void *element = getVectorItem (vector, i);
        if (cmp(element, max_elem)) {
            max_elem = element;
        }
    }
    void *max_copy = malloc(vector -> elem_size);
    if (max_copy == NULL) {
        return NULL;
    }
    memcpy(max_copy, max_elem, vector -> elem_size);
    return max_copy;
}

int removeDuplicatesList(GenericList *list, EqualsFunc cmp)
{
    if (list == NULL) {
        return -1;
    }
    if (list->head == NULL) {
        return 0; // пустой список
    }
    Node *current_node = list->head;
    while (current_node != NULL && current_node -> next != NULL) { // проходим по всем элементам списка
        Node *prevCheck = current_node;
        Node *check = current_node -> next;
        while (check != NULL) {
            if (cmp(current_node -> data, check -> data)) { // нашли дубликат - удаляем его
                Node *toRemove = check;
                prevCheck->next = check->next;
                check = check->next;
                free(toRemove->data); // освобождаем память удаляемого узла
                free(toRemove);
            } else { // переходим к следующему элементу
                prevCheck = check;
                check = check->next;
            }
        }
        
        current_node = current_node -> next;
    }
    
    return 0;
}

int removeDuplicatesVector(Vector *vector, EqualsFunc cmp)
{
    if (vector == NULL) {
        return -1;
    }
    if (vector -> size == 0) {
        return 0;
    }
    for (size_t i = 0; i < vector->size; i++) { // проходим по всем элементам вектора
        void *current_element = getVectorItem(vector, i);
        for (size_t j = i + 1; j < vector->size; ) { // проверяем все последующие элементы сравнивая с исходным
            void *check_element = getVectorItem(vector, j);
            if (cmp(current_element, check_element)) {
                void *removed = popVectorItem(vector, j); // дубликат - удаляем его
                if (removed != NULL) {
                    free(removed); // освобождаем память удаленного элемента
                }
                // не увеличиваем j, так как размер вектора уменьшился
            } else {
                j++;
            }
        }
    }
    return 0;
}