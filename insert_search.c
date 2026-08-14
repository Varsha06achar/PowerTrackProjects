#include "rbtree.h"

#include <stdlib.h>

extern tree_t *rbtree_nil(void);
extern tree_t *rbtree_search_node(tree_t *root, data_t item);
extern tree_t *rbtree_node_create(data_t item);
extern void rbtree_insert_fixup(tree_t **root, tree_t *z);

int insert(tree_t **root, data_t item)
{
    tree_t *nil = rbtree_nil();
    tree_t *parent = nil;
    tree_t *current;
    tree_t *z;

    if (root == NULL) return FAILURE;
    current = (*root != NULL) ? *root : nil;

    while (current != nil) {
        parent = current;
        if (item == current->data) return FAILURE;
        if (item < current->data) current = current->left;
        else current = current->right;
    }

    z = rbtree_node_create(item);
    if (z == NULL) return FAILURE;
    z->parent = parent;

    if (parent == nil) *root = z;
    else if (item < parent->data) parent->left = z;
    else parent->right = z;

    rbtree_insert_fixup(root, z);
    return SUCCESS;
}

int search(tree_t **root, data_t item)
{
    if (root == NULL || *root == NULL) return FAILURE;
    return rbtree_search_node(*root, item) != rbtree_nil() ? SUCCESS : FAILURE;
}
