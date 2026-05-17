#pragma once
#include "../posting.h"

typedef enum { TREE_AVL, TREE_RB, TREE_BTREE } TreeType;

// структура индекса, которая сможет хранить в себе любое дерево, которое мы зададим
typedef struct {
    void*    tree;
    TreeType type;
} Index;

// без комментариев..
Index* createIndex(TreeType type);

// берет слово, кладет его в дерево по ключу(слову) и привязывает к нему номер документа
void insertTerm(Index* idx, const char* term, int doc_id, const char* title);

// функция поиска, даем слово, она возвращает виктор номеров документов, где оно есть 
Vector* lookupTerm(const Index* idx, const char* term);

// функция записи массива слов в дерево(вызывает insertTerm в цикле)
void indexDocument(Index* idx, int doc_id, const char* title,
                       const char** tokens, int n_tokens);

// функция обхода, которая в зависимости от поданой функции, вызывает её для каждого узла
void traverseIndex(
    const Index* idx,
    void (*visit)(const char* key, Vector* postings, void* ctx),
    void* ctx
);

// вспомогательная функция, которая будет вызываться при обходе
void saveNode(const char* key, Vector* postings, void* ctx);

// сохранение готового дерева в файл .txt на диск
void saveIndex(const Index* idx, const char* path);

// быстрая загрузка из .txt при старте программы, чтобы не индексировать всё заново
Index* loadIndex(const char* path, TreeType type);

// очистка 
void freeIndex(Index* idx);
