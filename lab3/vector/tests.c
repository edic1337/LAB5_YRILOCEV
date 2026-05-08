#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
//#include <windows.h>
#include "generic.h"
#include "../comparators.h"

// структура Product
typedef struct {
    int id;
    char name[20];
    float price;
} Product;

// компаратор для структуры
int product_equals(const void* a, const void* b) {
    const Product* p1 = (const Product*)a;
    const Product* p2 = (const Product*)b;
    return p1->id == p2->id && 
           strcmp(p1->name, p2->name) == 0 && 
           floatEquals(&p1->price, &p2->price);
}

// базовые операции - добавление, удаление, поиск
void test_basic_operations() {
    printf("тест базовых операций вектора...\n");
    
    Vector* vec = createVector(sizeof(int));
    
    // добавляем числа от 0 до 9
    for (int i = 0; i < 10; i++) {
        appendVectorItem(vec, &i);
    }
    printf("   добавили 10 элементов\n");
    
    // проверяем размер
    assert(vec->size == 10);
    printf("   размер вектора: %zu\n", vec->size);
    
    // ищем число 5
    int search = 5;
    long index = findVectorItem(vec, &search, intEquals);
    assert(index == 5);
    printf("   нашли число 5 на позиции %ld\n", index);
    
    // удаляем элемент на позиции 3
    int* removed = popVectorItem(vec, 3);
    assert(*removed == 3);
    free(removed);
    assert(vec->size == 9);
    printf("   удалили элемент на позиции 3\n");
    
    vectorFree(vec);
    printf("   базовые операции работают\n\n");
}

// работа с разными типами
void test_different_types() {
    printf("тест разных типов данных...\n");
    
    // тест с float
    Vector* float_vec = createVector(sizeof(float));
    float f1 = 1.5f, f2 = 2.7f, f3 = 3.9f;
    appendVectorItem(float_vec, &f1);
    appendVectorItem(float_vec, &f2);
    appendVectorItem(float_vec, &f3);
    
    float search_f = 2.7f;
    long index = findVectorItem(float_vec, &search_f, floatEquals);
    assert(index == 1);
    printf("   float: нашли число 2.7\n");
    
    vectorFree(float_vec);
    
    // тест с double
    Vector* double_vec = createVector(sizeof(double));
    double d1 = 5.678, d2 = 9.123;
    appendVectorItem(double_vec, &d1);
    appendVectorItem(double_vec, &d2);
    
    double search_d = 9.123;
    index = findVectorItem(double_vec, &search_d, doubleEquals);
    assert(index == 1);
    printf("   double: нашли число 9.123\n");
    
    vectorFree(double_vec);
    
    // тест с char
    Vector* char_vec = createVector(sizeof(char));
    char ch1 = 'A', ch2 = 'B', ch3 = 'C';
    appendVectorItem(char_vec, &ch1);
    appendVectorItem(char_vec, &ch2);
    appendVectorItem(char_vec, &ch3);
    
    char search_c = 'B';
    index = findVectorItem(char_vec, &search_c, charEquals);
    assert(index == 1);
    printf("   char: нашли символ 'B'\n");
    
    vectorFree(char_vec);
    
    // тест с short
    Vector* short_vec = createVector(sizeof(short));
    short s1 = 100, s2 = 200;
    appendVectorItem(short_vec, &s1);
    appendVectorItem(short_vec, &s2);
    
    short search_s = 200;
    index = findVectorItem(short_vec, &search_s, shortEquals);
    assert(index == 1);
    printf("   short: нашли число 200\n");
    
    vectorFree(short_vec);
    printf("   разные типы данных работают\n\n");
}

// тест со структурами
void test_struct_operations() {
    printf("тест работы со структурами...\n");
    
    Vector* product_vec = createVector(sizeof(Product));
    
    // создаем несколько товаров
    Product p1 = {101, "VW", 999.99f};
    Product p2 = {102, "MAZDA", 25.50f};
    Product p3 = {103, "HONDA", 75.00f};
    
    // добавляем машины в вектор
    appendVectorItem(product_vec, &p1);
    appendVectorItem(product_vec, &p2);
    appendVectorItem(product_vec, &p3);
    printf("   добавили 3 товара в вектор\n");
    
    // ищем машину "MAZDA"
    Product search_product = {102, "MAZDA", 25.50f};
    long index = findVectorItem(product_vec, &search_product, product_equals);
    assert(index == 1);
    printf("   нашли машину 'MAZDA' на позиции %ld\n", index);
    
    // проверяем данные товара
    Product* retrieved = (Product*)getVectorItem(product_vec, 0);
    assert(retrieved != NULL);
    assert(retrieved->id == 101);
    assert(strcmp(retrieved->name, "VW") == 0);
    printf("   данные структур корректно хранятся\n");
    
    vectorFree(product_vec);
    printf("   работа со структурами проверена\n\n");
}

// граничные случаи
void test_edge_cases() {
    printf("тест граничных случаев вектора...\n");
    
    Vector* vec = createVector(sizeof(int));
    
    // пытаемся получить элемент из пустого вектора
    assert(getVectorItem(vec, 0) == NULL);
    printf("   пустой вектор: getItem возвращает NULL\n");
    
    // пытаемся удалить из пустого вектора
    assert(popVectorItem(vec, 0) == NULL);
    printf("   пустой вектор: popItem возвращает NULL\n");
    
    // добавляем один элемент
    int x = 42;
    appendVectorItem(vec, &x);
    printf("   добавили один элемент\n");
    
    // пытаемся получить элемент с неверным индексом
    assert(getVectorItem(vec, 1) == NULL);
    printf("   неверный индекс: getItem возвращает NULL\n");
    
    vectorFree(vec);
    printf("   граничные случаи обрабатываются\n\n");
}

// нагрузочный тест
void test_performance() {
    printf("простой нагрузочный тест вектора...\n");
    
    Vector* vec = createVector(sizeof(int));
    
    // заполняем массив числами [0; 999]
    for (int i = 0; i < 1000; i++) {
        appendVectorItem(vec, &i);
    }
    printf("   добавили 1000 элементов\n");
    
    // проверяем, что все на месте
    assert(vec->size == 1000);
    assert(*(int*)getVectorItem(vec, 500) == 500);
    printf("   все элементы добавлены корректно\n");
    
    // ищем различные элементы
    int search1 = 0;
    long index1 = findVectorItem(vec, &search1, intEquals);
    assert(index1 == 0);
    printf("   нашли первый элемент (0)\n");
    
    int search2 = 999;
    long index2 = findVectorItem(vec, &search2, intEquals);
    assert(index2 == 999);
    printf("   нашли последний элемент (999)\n");
    
    vectorFree(vec);
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