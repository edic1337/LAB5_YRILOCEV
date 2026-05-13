#include "btree.h"


BTreeNode* Create_BTreeNode() {
    BTreeNode* node = calloc(1, sizeof(BTreeNode));
    return node;
}


values_from_keys create_values_from_keys(int doc_id, const char* title) {
    values_from_keys result;

    result.doc_id = doc_id;
    strncpy(result.title, title, MAX_TITLE_LEN - 1);
    result.title[MAX_TITLE_LEN - 1] = '\0';

    return result;
}


BTree* createBTree() {
    BTree* result = malloc(sizeof(BTree));
    if (!result) {
        printf("Ошибка выделения памяти в структуре createbtree\n");
        return NULL;
    }

    BTreeNode* root = Create_BTreeNode();
    if (!root) {
        printf("Ошибка в выделении памяти creatBTree\n");
        free(result);
        return NULL;
    }
    root->key_count = 0;
    root->is_leaf = 1;


    result->unique_key = 0;
    result->root = root;

    return result;
}


void freeBTreeNode(BTreeNode* node) {
    if (node->is_leaf == 1) {
        for (int j = 0; j < node->key_count; j++) {
            free(node->keys[j]);
            vectorFree(node->postings[j]);
        }
        free(node);
        return;
    } 
    for (int i = 0; i < node->key_count + 1; i++) {
        freeBTreeNode(node->children[i]);
    }
    
    for (int j = 0; j < node->key_count; j++) {
            free(node->keys[j]);
            vectorFree(node->postings[j]);
        }

    free(node);
    return;
}


void freeBTree(BTree* tree) {
    if (!tree) {
        return;
    }
    freeBTreeNode(tree->root);
    free(tree);
}


void splitChild(BTreeNode* parent, int index) {
    BTreeNode* child = parent->children[index];
    int k = child->key_count / 2;

    // создаём правый узел
    BTreeNode* right = Create_BTreeNode();
    right->is_leaf = child->is_leaf;

    // копируем правые ключи
    right->key_count = child->key_count - k - 1;
    for (int i = 0; i < right->key_count; i++) {
        right->keys[i] = child->keys[i + k + 1];
        right->postings[i] = child->postings[i + k + 1];
    }

    // если не лист — копируем детей
    if (child->is_leaf == 0) {
        for (int i = 0; i <= right->key_count; i++) {
            right->children[i] = child->children[i + k + 1];
        }
    }

    // уменьшаем левый (старый child)
    child->key_count = k;

    // сдвигаем детей в parent
    for (int i = parent->key_count; i >= index + 1; i--) {
        parent->children[i + 1] = parent->children[i];
    }

    parent->children[index + 1] = right;

    // сдвигаем ключи в parent
    for (int i = parent->key_count - 1; i >= index; i--) {
        parent->keys[i + 1] = parent->keys[i];
        parent->postings[i + 1] = parent->postings[i];
    }

    // вставляем середину
    parent->keys[index] = child->keys[k];
    parent->postings[index] = child->postings[k];

    parent->key_count += 1;

    return;
}


void btreeInsert(BTree* tree, const char* key, int doc_id, const char* title) {
    if (!tree || !key || !title) {
        printf("Ошибка в функции btreeInseart");
        return ;
    }
    BTreeNode* node = tree->root;

    if (node->key_count == BTREE_MAX_KEYS) {
        BTreeNode* node_0 = Create_BTreeNode();
        node_0->is_leaf = 0;
        node_0->key_count = 0;
        node_0->children[0] = node;
        tree->root = node_0;
        splitChild(node_0, 0);
    }
    BTreeNode* curr_node = tree->root;
    int index;

    while (curr_node->is_leaf == 0) {
        index = 0;
        for (int i = 0; i < curr_node->key_count; i++) {
            
            if (strcmp(key, curr_node->keys[i]) == 0) {
                values_from_keys value = create_values_from_keys(doc_id, title);
                appendVectorItem(curr_node->postings[i], &value);
                return;
            }
            if (strcmp(key, curr_node->keys[i]) < 0) {
                index = i;
                break;
            }
            if (i == curr_node->key_count - 1) {
                index = i + 1;
            }
        }

        if (curr_node->children[index]->key_count == BTREE_MAX_KEYS) {
            splitChild(curr_node, index);
            if (strcmp(key, curr_node->keys[index]) > 0) {
                index += 1;
            }
            curr_node = curr_node->children[index];
        } else {
            curr_node = curr_node->children[index];
        }
    }

    for (int j = 0; j < curr_node->key_count; j++) {
    if (strcmp(key, curr_node->keys[j]) == 0) {
        values_from_keys value = create_values_from_keys(doc_id, title);
        appendVectorItem(curr_node->postings[j], &value);
        return;
        }
    }

    int i = curr_node->key_count - 1;

    while (i >= 0 && strcmp(key, curr_node->keys[i]) < 0) {
        curr_node->keys[i + 1] = curr_node->keys[i];
        curr_node->postings[i + 1] = curr_node->postings[i];
        i -= 1;
    }

    curr_node->keys[i + 1] = strdup(key);

    Vector* list = createVector(sizeof(values_from_keys));
    values_from_keys value = create_values_from_keys(doc_id, title);

    appendVectorItem(list, &value);
    curr_node->postings[i + 1] = list;

    curr_node->key_count += 1;
    tree->unique_key += 1;
}


Vector* btreeSearch(const BTree* tree, const char* key) {
    BTreeNode* node = tree->root;
    if (!node || node->key_count == 0) {
        return NULL;
    }

    int curr_value;

    while (node->is_leaf != 1) {
        int index = 0;
        for (int i = 0; i < node->key_count; i++) {

            curr_value = strcmp(key, node->keys[i]);

            if (curr_value == 0) {
                return node->postings[i];
            }

            if (curr_value < 0) {
                index = i;
                break;
            }

            if (i == node->key_count - 1) {
                index = i + 1;
            }
        }
        node = node->children[index];
    }

    for (int j = 0; j < node->key_count; j++) {
        curr_value = strcmp(key, node->keys[j]);

        if (curr_value == 0) {
            return node->postings[j];
        }
    }

    return NULL;
}


void traverseNode(BTreeNode* node,
     void (*visit)(const char*, Vector*, void*), void* ctx) {
        if (!node) {
            return;
        }

        if (node->is_leaf == 1) {
            for (int i = 0; i < node->key_count; i++) {
                visit(node->keys[i], node->postings[i], ctx);
            }
            return;
        }

        for (int j = 0; j < node->key_count; j++) {
            traverseNode(node->children[j], visit, ctx);
            visit(node->keys[j], node->postings[j], ctx);
        }
        traverseNode(node->children[node->key_count], visit, ctx);
        return;
}


void btreeTraverse(const BTree* tree,
    void (*visit)(const char* key, Vector* postings, void* ctx), void* ctx) {
        BTreeNode* node = tree->root;
        traverseNode(node, visit, ctx); 
}