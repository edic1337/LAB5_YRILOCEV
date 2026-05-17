#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "index.h"
#include "../btree/btree.h"
#include "../avl/avl.h"
#include "../rbtree/rbtree.h"

Index* createIndex(TreeType type) {
    Index* idx = (Index*)malloc(sizeof(Index));
    if (idx == NULL) {
        printf("memory failed");
        return NULL;
    }
    idx -> type = type;
    switch (type) {
        case TREE_BTREE:
            idx -> tree = createBTree();
            break;
        case TREE_AVL:
            idx -> tree = createAVLTree();
            break;
        case TREE_RB:
            idx -> tree = createRBTree();
            break;
    }
    return idx;
}

void insertTerm(Index* idx, const char* term, int doc_id, const char* title) {
    if (idx == NULL || idx -> tree == NULL) {
        printf("idx is NULL or tree is NULL");
        return;
    }
    switch(idx -> type) {
        case TREE_BTREE:
            btreeInsert(idx -> tree, term, doc_id, title);
            break;
        case TREE_AVL:
            avlInsert(idx -> tree, term, doc_id, title);
            break;
        case TREE_RB:
            rbInsert(idx -> tree, term, doc_id, title);
            break;
    }
}

Vector* lookupTerm(const Index* idx, const char* term) {
    if (idx == NULL|| idx -> tree == NULL) {
        printf("idx is NULL or tree is NULL");
        return NULL;
    }

    switch (idx -> type) {
        case TREE_BTREE:
            return btreeSearch((BTree*)idx -> tree, term);
        case TREE_AVL:
            return avlSearch((AVLTree*)idx -> tree, term);
        case TREE_RB:
            return rbSearch((RBTree*)idx -> tree, term);
        default:
            return NULL;
    }
}

void indexDocument(Index* idx, int doc_id, const char* title,
                       const char** tokens, int n_tokens) {
    if (idx == NULL|| idx -> tree == NULL) {
        printf("idx is NULL or tree is NULL");
        return;
    }
    for (int i = 0; i < n_tokens; i++) {
        insertTerm(idx, tokens[i], doc_id, title);
    }
    return;
                       }

void traverseIndex(
    const Index* idx,
    void (*visit)(const char* key, Vector* postings, void* ctx),
    void* ctx
) {
    if (idx == NULL|| idx -> tree == NULL) {
        printf("idx is NULL od tree is NULL");
        return ;
    }
    switch (idx -> type) {
        case TREE_BTREE:
            btreeTraverse(idx -> tree, visit, ctx);
            break;
        case TREE_AVL:
            avlTraverse(idx -> tree, visit, ctx);
            break;  
        case TREE_RB:
            rbTraverse(idx -> tree, visit, ctx); 
            break;  
    }
}

void saveNode(const char* key, Vector* postings, void* ctx) {
    FILE* file = (FILE*)ctx;
    fprintf(file, "%s %d\n", key, postings -> size); // записывем слово и кол во документов
    for (int i = 0; i < postings -> size; i++) {
        values_from_keys* val = (values_from_keys*)getVectorItem(postings, i);
        // пишем ID, затем знак табуляции, затем заголовок и перенос строки
        fprintf(file, "%d\t%s\n", val -> doc_id, val -> title);
    }
    fprintf(file, "\n");
}

void saveIndex(const Index* idx, const char* path) {
    FILE* file = fopen(path, "w");
    if (file == NULL) {
        printf("cant open file\n");
        return;
    }
    traverseIndex(idx, saveNode, file);
    fclose(file);
    printf("index seved to: %s\n", path);
}

Index* loadIndex(const char* path, TreeType type) {
    FILE* file  = fopen(path, "r");
    if (file == NULL) {
        printf("cant open file");
        return NULL;
    }
    Index* idx = createIndex(type);
    if (idx == NULL) {
        fclose(file);
        return NULL;
    }
    char term[512];
    int count;
    while (fscanf(file, "%511s %d", term, &count) == 2) {
        for (int i = 0; i < count; i++) {
            int doc_id;
            char title[MAX_TITLE_LEN];
            if (fscanf(file, "%d\t%[^\n]", &doc_id, title) == 2) {
                insertTerm(idx, term, doc_id, title);
            }
        }
    }
    fclose(file);
    printf("index load in %s\n", path);
    return idx;
}

void freeIndex(Index* idx) {
    if (idx -> tree == NULL) {
        free(idx);
        return;
    }
    switch (idx -> type) {
            case TREE_BTREE:
                freeBTree((BTree*)idx -> tree);
                break;
            case TREE_AVL:
                freeAVLTree((AVLTree*)idx -> tree);
                break;
            case TREE_RB:
                freeRBTree((RBTree*)idx -> tree);
                break;
        }
    free(idx);
}