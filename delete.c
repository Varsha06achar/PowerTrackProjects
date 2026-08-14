#include "rbtree.h"
#include <stdlib.h>

extern tree_t *rbtree_nil(void);
extern tree_t *rbtree_search_node(tree_t *root, data_t item);
extern tree_t *rbtree_minimum_node(tree_t *node);
extern tree_t *rbtree_maximum_node(tree_t *node);
extern void rbtree_transplant(tree_t **root, tree_t *u, tree_t *v);
extern void rbtree_delete_fixup(tree_t **root, tree_t *x);

static int delete_node(tree_t **root, tree_t *z)
{
    tree_t *nil = rbtree_nil();
    tree_t *y = z;
    tree_t *x;
    color_t original_color = y->color;

    if (z->left == nil) {
        x = z->right;
        rbtree_transplant(root, z, z->right);
    } else if (z->right == nil) {
        x = z->left;
        rbtree_transplant(root, z, z->left);
    } else {
        y = rbtree_minimum_node(z->right);
        original_color = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        } else {
            rbtree_transplant(root, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        rbtree_transplant(root, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    free(z);
    if (original_color == BLACK) {
        if (*root == NULL) nil->parent = nil;
        else {
            rbtree_delete_fixup(root, x);
            (*root)->color = BLACK;
            (*root)->parent = nil;
        }
    }
    return SUCCESS;
}

int delete(tree_t **root, data_t item)
{
    tree_t *z;
    if (root == NULL || *root == NULL) return FAILURE;
    z = rbtree_search_node(*root, item);
    if (z == rbtree_nil()) return FAILURE;
    return delete_node(root, z);
}

int delete_minimum(tree_t **root)
{
    if (root == NULL || *root == NULL) return FAILURE;
    return delete_node(root, rbtree_minimum_node(*root));
}

int delete_maximum(tree_t **root)
{
    if (root == NULL || *root == NULL) return FAILURE;
    return delete_node(root, rbtree_maximum_node(*root));
}
