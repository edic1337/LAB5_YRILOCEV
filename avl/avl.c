#include "avl.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


// получение высоты узла
static int getHeight(AVLNode* node) {
    if (node == NULL) {
        return 0;
    }
    return node->height;
}

// вычисление баланса
static int getBalance(AVLNode* node) {
    if (node == NULL) {
        return 0;
    }
    return getHeight(node->left) - getHeight(node->right);
}

// обновление высоты узла на основе детей
static void updateHeight(AVLNode* node) {
    if (node != NULL) {
        int hl = getHeight(node->left);
        int hr = getHeight(node->right);
        int max_h;
        
        if (hl > hr) {
            max_h = hl;
        } else {
            max_h = hr;
        }
        
        node->height = max_h + 1;
    }
}

// создание нового узла
static AVLNode* createAVLNode(const char* key, int doc_id, const char* title) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    if (node == NULL) {
        return NULL;
    }

    node->key = strdup(key); 
    if (node->key == NULL) {
        free(node);
        return NULL;
    }

    node->height = 1; // новый узел - всегда лист
    node->left = NULL;
    node->right = NULL;
    
    node->postings = createPostingList();
    appendPosting(node->postings, doc_id, title);

    return node;
}


// простой правый поворот (LL-дисбаланс)
static AVLNode* rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    updateHeight(y);
    updateHeight(x);

    return x; 
}

// простой левый поворот (RR-дисбаланс)
static AVLNode* rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    updateHeight(x);
    updateHeight(y);

    return y; 
}

// рекурсивная функция для вставки и балансировки
static AVLNode* insertRecursive(AVLNode* node, const char* key, int doc_id, const char* title, int* is_new_key) {
    // вставка
    if (node == NULL) {
        *is_new_key = 1;
        return createAVLNode(key, doc_id, title);
    }

    int cmp = strcmp(key, node->key);
    if (cmp < 0) {
        node->left = insertRecursive(node->left, key, doc_id, title, is_new_key);
    } else if (cmp > 0) {
        node->right = insertRecursive(node->right, key, doc_id, title, is_new_key);
    } else {
        // ключ уже есть в дереве
        appendPosting(node->postings, doc_id, title);
        *is_new_key = 0;
        return node;
    }

    updateHeight(node);

    // балансировка
    int balance = getBalance(node);

    // Случай LL
    if (balance > 1) {
        if (strcmp(key, node->left->key) < 0) {
            return rotateRight(node);
        }
    }

    // Случай RR
    if (balance < -1) {
        if (strcmp(key, node->right->key) > 0) {
            return rotateLeft(node);
        }
    }

    // Случай LR
    if (balance > 1) {
        if (strcmp(key, node->left->key) > 0) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
    }

    // Случай RL
    if (balance < -1) {
        if (strcmp(key, node->right->key) < 0) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
    }

    return node;
}

// создание дерева
AVLTree* createAVLTree(void) {
    AVLTree* tree = (AVLTree*)malloc(sizeof(AVLTree));
    if (tree != NULL) {
        tree->root = NULL;
        tree->size = 0;
    }
    return tree;
}

// вставка узла
void avlInsert(AVLTree* tree, const char* key, int doc_id, const char* title) {
    if (tree == NULL) {
        return;
    }
    if (key == NULL) {
        return;
    }
    
    int is_new_key = 0;
    tree->root = insertRecursive(tree->root, key, doc_id, title, &is_new_key);
    
    if (is_new_key == 1) {
        tree->size = tree->size + 1;
    }
}

// поиск по дереву
Vector* avlSearch(const AVLTree* tree, const char* key) {
    if (tree == NULL) {
        return NULL;
    }
    if (key == NULL) {
        return NULL;
    }
    
    AVLNode* curr = tree->root;
    while (curr != NULL) {
        int cmp = strcmp(key, curr->key);
        if (cmp == 0) {
            return curr->postings;
        }
        
        if (cmp < 0) {
            curr = curr->left;
        } else {
            curr = curr->right;
        }
    }
    return NULL;
}

// вспомогательная функция для обхода дерева
static void traverseRecursive(AVLNode* node, void (*visit)(const char*, Vector*, void*), void* ctx) {
    if (node == NULL) {
        return;
    }
    traverseRecursive(node->left, visit, ctx);
    visit(node->key, node->postings, ctx);
    traverseRecursive(node->right, visit, ctx);
}

// обход дерева
void avlTraverse(const AVLTree* tree, void (*visit)(const char* key, Vector* postings, void* ctx), void* ctx) {
    if (tree != NULL) {
        if (visit != NULL) {
            traverseRecursive(tree->root, visit, ctx);
        }
    }
}

// удаление узла
static void freeNodes(AVLNode* node) {
    if (node == NULL) {
        return;
    }
    freeNodes(node->left);
    freeNodes(node->right);
    
    if (node->key != NULL) {
        free(node->key);
    }
    if (node->postings != NULL) {
        vectorFree(node->postings);
    }
    free(node);
}

// удаление дерева
void freeAVLTree(AVLTree* tree) {
    if (tree != NULL) {
        freeNodes(tree->root);
        free(tree);
    }
}