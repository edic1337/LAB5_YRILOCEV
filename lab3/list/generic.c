#include "generic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GenericList *createList(size_t elem_size) // выделяем память под характеристики списка и храним их
{
    if (elem_size == 0){
        printf("Некоректный размер");
        return NULL;
    }
    GenericList *list = malloc(sizeof(GenericList)); // сохранили адресс выделенной памяти 
    list -> head = NULL;
    list -> elem_size = elem_size;
    return list;
}

void appendItem(GenericList *list, void *data)
{
    Node *new_node = malloc(sizeof(Node)); // новый узел
    new_node -> data = malloc(list -> elem_size);
    memcpy(new_node -> data, data, list -> elem_size); // копируем побайтово данные добавляемого элемента в новый узел
    new_node -> next = NULL;
    if (list -> head == NULL) { // список пустой - новый узел становится головой
        list -> head = new_node;
     } else {
        Node *current = list -> head;
        while (current -> next != NULL) { // проходим по всем узлам, начиная с головы
            current = current->next;
        }
        current -> next = new_node; // последний узел теперь ссылается на добавленный 
    }
}

int findItem(GenericList *list, void *value, EqualsFunc cmp)
{
    if (list == NULL || value == NULL || cmp == NULL) {
        return -1;
    }  
    Node *current = list -> head;
    int index = 0;
    while (current != NULL) { // проходим по всем узлам 
        if (cmp(current -> data, value)) { // сравниваем атрибут data 
            return index;
        }
        index++;
        current = current -> next;
    }
    return -1;  // не нашли
}

void *popItem(GenericList *list, size_t index)
{
    if (list == NULL || list -> head == NULL) {
        return NULL;
    }
    Node *current = list -> head;
    Node *prev = NULL;
    size_t current_index = 0;
    while (current != NULL && current_index < index) { // идем до нужного элемента 
    prev = current ;
    current = current -> next;
    current_index++;
    }
    if (current == NULL) { // проверка на валидность индекса
        return NULL;
    }
    if (prev == NULL) {
        list->head = current->next;  // поменяли указатель на следующий для головы
    } else {
        prev -> next = current -> next; // поменяли указатель на следующий для остальных
    }
    void *d_copy = malloc(list -> elem_size); // тут будем хранить копию данных
    if (d_copy == NULL) {
        return NULL;
    }
    memcpy(d_copy, current -> data, list -> elem_size); // само копирование 
    free(current -> data); // освобождаем ячейки памяти от значения удаленного элемента
    free( current);
    return d_copy;
}

void freeList(GenericList *list)
{
    if (list == NULL) {
        return;
    }
    Node *current = list -> head;
    while (current != NULL) {
        Node *next = current -> next;
        free(current -> data);
        free(current);
        current = next; // все почистили, но нельзя забыть кто следующий 
        }
    free(list);
}

unsigned int listLength(GenericList *list)
{
    if (list == NULL) {
    return 0;
    }
    Node *current = list -> head;
    unsigned int count = 0;
    while (current != NULL) {  // тот же обход что и в поиске, но с последним 
        count++;
        current = current -> next;
    }
    return count;
}
