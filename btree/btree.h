#pragma once

#include "../posting.h"

#define BTREE_T  3
#define BTREE_MAX_KEYS (2 * BTREE_T - 1)
#define BTREE_MAX_CH   (2 * BTREE_T)

typedef struct BTreeNode {
    char* keys[BTREE_MAX_KEYS];
    Vector* postings[BTREE_MAX_KEYS];
    struct BTreeNode* children[BTREE_MAX_CH];
    int key_count;
    int is_leaf;
} BTreeNode;

typedef struct {
    BTreeNode* root;
    int unique_key;
} BTree;

typedef struct values_from_keys {
    int doc_id;
    char title[MAX_TITLE_LEN];
} values_from_keys;

BTree* createBTree();

void freeBTree(BTree* tree);

void freeBTreeNode(BTreeNode* node);

void splitChild(BTreeNode* node, int index); // узел родителя, индекс ребенка для сплита

void btreeInsert(BTree* tree, const char* key, int doc_id, const char* title);

Vector* btreeSearch(const BTree* tree, const char* key);

void traverseNode(BTreeNode* node,
     void (*visit)(const char*, Vector*, void*), void* ctx);

void btreeTraverse(
    const BTree* tree,
    void (*visit)(const char* key, Vector* postings, void* ctx),
    void* ctx
);