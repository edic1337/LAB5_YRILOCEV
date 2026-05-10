#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
//#include <windows.h>
#include "generic.h"
#include "../comparators.h"

// Тестовая структура
typedef struct {
    int id;
    char name[20];
    float price;
} Product;

// Компаратор для структуры Product
int product_equals(const void* a, const void* b) {
    const Product* p1 = (const Product*)a;
    const Product* p2 = (const Product*)b;
    return p1->id == p2->id && 
           strcmp(p1->name, p2->name) == 0 && 
           floatEquals(&p1->price, &p2->price);
}

// базовые операции - добавление, удаление, поиск
void test_basic_operations() {
    printf("1. тест базовых операций списка...\n");
    
    GenericList* list = createList(sizeof(int));
    
    // добавляем числа 10, 20, 30
    int a = 10, b = 20, c = 30;
    appendItem(list, &a);
    appendItem(list, &b);
    appendItem(list, &c);
    printf("   добавили 3 элемента: 10, 20, 30\n");
    
    // проверяем размер
    assert(listLength(list) == 3);
    printf("   размер списка: %d\n", listLength(list));
    
    // ищем число 20
    int search = 20;
    unsigned int index = findItem(list, &search, intEquals);
    assert(index == 1);
    printf("   нашли число 20 на позиции %d\n", index);
    
    // удаляем элемент на позиции 1 (число 20)
    int* removed = popItem(list, 1);
    assert(*removed == 20);
    free(removed);
    assert(listLength(list) == 2);
    printf("   удалили элемент на позиции 1\n");
    
    freeList(list);
    printf("   базовые операции работают\n\n");
}

// работа с разными типами
void test_different_types() {
    printf("2. тест разных типов данных...\n");
    
    // тест с float
    GenericList* float_list = createList(sizeof(float));
    float f = 2.5f;
    appendItem(float_list, &f);
    
    float search_f = 2.5f;
    unsigned int index = findItem(float_list, &search_f, floatEquals);
    assert(index == 0);
    printf("   float: нашли число 2.5\n");
    
    freeList(float_list);
    
    // тест с char
    GenericList* char_list = createList(sizeof(char));
    char ch = 'X';
    appendItem(char_list, &ch);
    
    char search_ch = 'X';
    index = findItem(char_list, &search_ch, charEquals);
    assert(index == 0);
    printf("   char: нашли символ 'X'\n");
    
    freeList(char_list);
    printf("   разные типы данных работают\n\n");

    // тест с double
    GenericList* double_list = createList(sizeof(double));
    double d1 = 1.23456, d2 = 7.89012;
    appendItem(double_list, &d1);
    appendItem(double_list, &d2);
    
    double search_d = 7.89012;
    index = findItem(double_list, &search_d, doubleEquals);
    assert(index == 1);
    printf("   double: нашли число 7.89012\n");
    
    freeList(double_list);

    // тест с long
    GenericList* long_list = createList(sizeof(long));
    long l1 = 1000L, l2 = 2000L;
    appendItem(long_list, &l1);
    appendItem(long_list, &l2);
    
    long search_l = 2000L;
    index = findItem(long_list, &search_l, longEquals);
    assert(index == 1);
    printf("   long: нашли число 2000\n");
    
    freeList(long_list);
}

// тест со структурами
void test_struct_operations() {
    printf("  работа со структурами...\n");
    
    GenericList* product_list = createList(sizeof(Product));
    
    // создаем несколько машин
    Product p1 = {1, "AUDI", 2.5f};
    Product p2 = {2, "BMW", 1.8f};
    Product p3 = {3, "OPEL", 3.2f};
    
    // добавляем машины в список
    appendItem(product_list, &p1);
    appendItem(product_list, &p2);
    appendItem(product_list, &p3);
    printf("   добавили 3 машины в список\n");
    
    // ищем "BMW"
    Product search_product = {2, "BMW", 1.8f};
    unsigned int index = findItem(product_list, &search_product, product_equals);
    assert(index == 1);
    printf("   нашли машину 'BMW' на позиции %d\n", index);
    
    // проверяем данные
    Node* current = product_list->head;
    for (int i = 0; i < 3; i++) {
        Product* product = (Product*)current->data;
        assert(product != NULL);
        current = current->next;
    }
    printf("   все машины корректно хранятся в списке\n");
    
    freeList(product_list);
    printf("   работа со структурами)\n\n");
}

// граничные случаи
void test_edge_cases() {
    printf("3. тест граничных случаев списка...\n");
    
    GenericList* list = createList(sizeof(int));
    
    // пытаемся найти в пустом списке
    int x = 100;
    unsigned int index = findItem(list, &x, intEquals);
    assert(index == (unsigned int)-1);
    printf("   пустой список: findItem возвращает -1\n");
    
    // пытаемся удалить из пустого списка
    assert(popItem(list, 0) == NULL);
    printf("   пустой список: popItem возвращает NULL\n");
    
    // добавляем один элемент
    appendItem(list, &x);
    printf("   добавили один элемент\n");
    
    // пытаемся удалить с неверным индексом
    assert(popItem(list, 5) == NULL);
    printf("   неверный индекс: popItem возвращает NULL\n");
    
    freeList(list);
    printf("   граничные случаи обрабатываются\n\n");
}

// нагрузочный тест
void test_performance() {
    printf("4. простой нагрузочный тест списка...\n");
    
    GenericList* list = createList(sizeof(int));
    
    // добавляем 500 элементов
    for (int i = 0; i < 500; i++) {
        appendItem(list, &i);
    }
    printf("   добавили 500 элементов\n");
    
    // проверяем размер
    assert(listLength(list) == 500);
    printf("   размер списка: %d\n", listLength(list));
    
    // ищем последний элемент
    int search = 499;
    unsigned int index = findItem(list, &search, intEquals);
    assert(index == 499);
    printf("   нашли последний элемент\n");
    
    freeList(list);
    printf("   нагрузочный тест пройден\n\n");
}

int main() {
    //SetConsoleOutputCP(CP_UTF8);
    printf("ПРОСТЫЕ ТЕСТЫ\n\n");
    
    test_basic_operations();
    test_different_types();
    test_struct_operations();
    test_edge_cases();
    test_performance();

    printf("PASSED!\n");
    return 0;
}