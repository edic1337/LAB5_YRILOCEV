// Тут все примерно, можете подстроить под себя, главное, чтобы интерфейс был таким же, как в avl и btree
#pragma once

#include "../posting.h"

typedef enum { RB_RED, RB_BLACK } RBColor;

typedef struct RBNode {
    char*           key;
    RBColor         color;
    Vector*         postings;
    struct RBNode*  left;
    struct RBNode*  right;
    struct RBNode*  parent;
} RBNode;

typedef struct {
    RBNode* root;
    RBNode* nil;   /* sentinel-узел (чёрный лист) */
    int     size;
} RBTree;

static RBNode* createRBNode(RBNode* nil, const char* key, int doc_id, const char* title);
RBTree* createRBTree(void);

static void freeRBTreeHelper(RBNode* node, RBNode* nil);
void    freeRBTree(RBTree* tree);

static void leftRotate(RBTree* tree, RBNode* node);
static void rightRotate(RBTree* tree, RBNode* node);
static void rbInsertFixup(RBTree* tree, RBNode * node);

void    rbInsert(RBTree* tree, const char* key, int doc_id, const char* title);
static void rbTraverseHelper(RBNode* node, RBNode* nil, void (*visit)(const char* key, Vector* postings, void* ctx), void* ctx);
Vector* rbSearch(const RBTree* tree, const char* key);
void    rbTraverse(
    const RBTree* tree,
    void (*visit)(const char* key, Vector* postings, void* ctx),
    void* ctx
);
