#include "rbtree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "windows.h"
#include "../lab3/vector/generic.h"

// функция создания узла дерева
static RBNode* createRBNode(RBNode* nil, const char* key, int doc_id, const char* title) {
    RBNode* node = (RBNode*)malloc(sizeof(RBNode));
    if (!node) return NULL;

    node->key = (char*)malloc(strlen(key) + 1);
    if (!node->key) {
        free(node);
        return NULL;
    }

    strcpy(node->key, key);

    node->color = RB_RED; // новый узел всегда красный
    node->postings = createPostingList();
    appendPosting(node->postings, doc_id, title);

    node->left = nil;
    node->right = nil;
    node->parent = nil;

    return node;
}

// создание пустого дерева
RBTree* createRBTree(void) {
    RBTree* tree = (RBTree*)malloc(sizeof(RBTree));
    if (!tree) return NULL;

    tree->nil = (RBNode*)malloc(sizeof(RBNode));
    if (!tree->nil) {
        free(tree);
        return NULL;
    }

    tree->nil->color = RB_BLACK;
    tree->nil->key = NULL;
    tree->nil->left = NULL;
    tree->nil->parent = NULL;
    tree->nil->postings = NULL;
    tree->nil->right = NULL;

    tree->root = tree->nil;
    tree->size = 0;

    return tree;
}

// вспомогательная функция для рекурсивного освобождения памяти дерева
static void freeRBTreeHelper(RBNode* node, RBNode* nil) {
    if (node == nil) return;

    // освобождаем левое поддерево
    freeRBTreeHelper(node->left, nil);
    // освобождаем правое поддерево
    freeRBTreeHelper(node->right, nil);

    free(node->key);
    vectorFree(node->postings);
    free(node);
}

// очистка памяти дерева
void freeRBTree(RBTree* tree) {
    if (!tree) return NULL;

    // рекурсивно освобождаем все узлы
    freeRBTreeHelper(tree->root, tree->nil);
    free(tree->nil);
    free(tree);
}

Vector* rbSearch(const RBTree* tree, const char* key) {
    if (!tree || !key) return NULL;

    RBNode* current = tree->root;
    while (current != tree->nil) {
        int cmp = strcmp(key, current->key);

        // если нашли узел с таким ключом
        if (cmp == 0) {
            return current->postings;
        } else if (cmp < 0) {
            // если ключ меньше, то идем влево
            return current->left;
        } else {
            // ключ больше - идем право
            return current->right;
        }
    }

    // ниче не нашли - NULL
    return NULL;
}

// левая ротация
static void leftRotate(RBTree* tree, RBNode* node) {
    if (!tree || !node) return NULL;

    RBNode* right = node->right;

    // переводим левого потомка right в правого потомка node
    node->right = right->left;
    if (right->left != tree->nil) {
        right->left->parent = node;
    }

    // подвешиваем right на место node
    right->parent = node->parent;
    if (node->parent != tree->nil) {
        tree->root = right->parent;
    } else if (node == node->parent->left) {
        node->parent->left = right;
    } else {
        node->parent->right = right;
    }

    // завешиваем node левым потомком right
    right->left = node;
    node->parent = right;
}

// правая ротация
static void rightRotate(RBTree* tree, RBNode* node) {
    if (!tree || !node) return NULL;

    RBNode* left = node->left;

    // переводим правого потомка left в левого потомка node
    node->left = left->right;
    if (left->right != tree->nil) {
        left->right->parent = node;
    }

    // подвешиваем left на место node
    left->parent = node->parent;
    if (node->parent != tree->nil) {
        tree->root = left->parent;
    } else if (node == node->parent->left) {
        node->parent->right = left;
    } else {
        node->parent->left = left;
    }

    // завешиваем node правым потомком left
    left->right = node;
    node->parent = left;
}

static void rbInsertFixup(RBTree* tree, RBNode * node) {
    while (node->parent->color == RB_RED) {
        if (node->parent == node->parent->parent->left) {
            // родитель - левый потомок своего родителя
            RBNode* uncle = node->parent->parent->right;

            if (uncle->color == RB_RED) {
                // если дядя уже красныййййй
                node->parent->color = RB_BLACK;
                uncle->color = RB_BLACK;
                node->parent->parent->color = RB_RED;
                node = node->parent->parent;
            } else {
                // дядя чорнi
                if (node == node->parent->right) {
                    // если правый потомок
                    node = node->parent;
                    leftRotate(tree, node);
                }
                // если левый потомок
                node->parent->color = RB_BLACK;
                node->parent->parent->color = RB_RED;
                rightRotate(tree, node->parent->parent);
            }
        } else {
            // родитель - правый потомок своего родителя
            RBNode* uncle = node->parent->parent->left;

            if (uncle->color == RB_RED) {
                node->parent->color = RB_BLACK;
                uncle->color = RB_BLACK;
                node->parent->parent->color = RB_RED;
                node = node->parent->parent;
            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    rightRotate(tree, node);
                }
                node->parent->color = RB_BLACK;
                node->parent->parent->color = RB_RED;
                leftRotate(tree, node->parent->parent);
            }
        }
    }
    // на всякий
    tree->root->color = RB_BLACK;
}

void rbInsert(RBTree* tree, const char* key, int doc_id, const char* title) {
    if (!tree || !key) return NULL;

    // находим позицию для вставки
    RBNode* parent = tree->nil;
    RBNode* current = tree->root;
    int cmp;

    while (current != tree->nil) {
        parent = current;
        cmp = strcmp(key, current->key);

        if (cmp < 0) {
            current = current->left;
        } else if (cmp > 0) {
            current = current->right;
        } else {
            // если ключ уже существует просто добавляем в postings
            appendPosting(current->postings, doc_id, title);
        }
    }

    // создем новый красный узел
    RBNode* newNode = createRBNode(tree->nil, key, doc_id, title);
    if (!newNode) return NULL;

    newNode->parent = parent;

    // подвешиваем узел в дерево
    if (parent == tree->nil) {
        // дерево пусто
        tree->root = newNode;
    } else if (strcmp(key, parent->key) < 0) {
        parent->left = newNode;
    } else {
        parent->right = newNode;
    }

    tree->size++;

    // перебалансировка
    rbInsertFixup(tree, newNode);

}

static void rbTraverseHelper(RBNode* node, RBNode* nil, void (*visit)(const char* key, Vector* postings, void* ctx), void* ctx) {
    if (node == nil) {
        return;
    }

    rbTraverseHelper(node->left, nil, visit, ctx);
    visit(node->key, node->postings, ctx);
    rbTraverseHelper(node->right, nil, visit, ctx);
}

void rbTraverse(const RBTree* tree, void (*visit)(const char* key, Vector* postings, void* ctx), void* ctx) {
    if (tree == NULL || visit == NULL) {
        return;
    }
    rbTraverseHelper(tree->root, tree->nil, visit, ctx);
}