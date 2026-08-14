#include "rbtree.h"
#include <stdlib.h>

static tree_t nil_node = {0, BLACK, &nil_node, &nil_node, &nil_node};
static tree_t *NIL = &nil_node;

tree_t *rbtree_nil(void) { return NIL; }

tree_t *rbtree_node_create(data_t item)
{
    tree_t *node = malloc(sizeof(*node));

    if (node == NULL)
        return NULL;

    node->data = item;
    node->color = RED;
    node->left = NIL;
    node->right = NIL;
    node->parent = NIL;

    return node;
}

tree_t *rbtree_root_node(tree_t **root)
{
    return (root != NULL && *root != NULL) ? *root : NIL;
}

void rbtree_left_rotate(tree_t **root, tree_t *x)
{
    tree_t *y = x->right;

    x->right = y->left;
    if (y->left != NIL)
        y->left->parent = x;

    y->parent = x->parent;

    if (x->parent == NIL)
        *root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;
}

void rbtree_right_rotate(tree_t **root, tree_t *y)
{
    tree_t *x = y->left;

    y->left = x->right;
    if (x->right != NIL)
        x->right->parent = y;

    x->parent = y->parent;

    if (y->parent == NIL)
        *root = x;
    else if (y == y->parent->right)
        y->parent->right = x;
    else
        y->parent->left = x;

    x->right = y;
    y->parent = x;
}

void rbtree_insert_fixup(tree_t **root, tree_t *z)
{
    while (z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            tree_t *y = z->parent->parent->right;

            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    rbtree_left_rotate(root, z);
                }

                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rbtree_right_rotate(root, z->parent->parent);
            }
        } else {
            tree_t *y = z->parent->parent->left;

            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rbtree_right_rotate(root, z);
                }

                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rbtree_left_rotate(root, z->parent->parent);
            }
        }
    }

    (*root)->color = BLACK;
    (*root)->parent = NIL;
}

tree_t *rbtree_search_node(tree_t *root, data_t item)
{
    while (root != NIL) {
        if (item == root->data)
            return root;

        if (item < root->data)
            root = root->left;
        else
            root = root->right;
    }

    return NIL;
}

tree_t *rbtree_minimum_node(tree_t *node)
{
    while (node->left != NIL)
        node = node->left;

    return node;
}

tree_t *rbtree_maximum_node(tree_t *node)
{
    while (node->right != NIL)
        node = node->right;

    return node;
}

void rbtree_transplant(tree_t **root, tree_t *u, tree_t *v)
{
    if (u->parent == NIL)
        *root = (v == NIL) ? NULL : v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;

    /*
     * NIL->parent is deliberately updated during deletion so the
     * delete-fixup routine knows the parent of an NIL replacement.
     */
    v->parent = u->parent;
}

void rbtree_delete_fixup(tree_t **root, tree_t *x)
{
    while (x != rbtree_root_node(root) && x->color == BLACK) {
        if (x == x->parent->left) {
            tree_t *w = x->parent->right;

            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rbtree_left_rotate(root, x->parent);
                w = x->parent->right;
            }

            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    rbtree_right_rotate(root, w);
                    w = x->parent->right;
                }

                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                rbtree_left_rotate(root, x->parent);
                x = rbtree_root_node(root);
            }
        } else {
            tree_t *w = x->parent->left;

            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rbtree_right_rotate(root, x->parent);
                w = x->parent->left;
            }

            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    rbtree_left_rotate(root, w);
                    w = x->parent->left;
                }

                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rbtree_right_rotate(root, x->parent);
                x = rbtree_root_node(root);
            }
        }
    }

    x->color = BLACK;
}

static void destroy_rec(tree_t *node)
{
    if (node == NIL)
        return;

    destroy_rec(node->left);
    destroy_rec(node->right);
    free(node);
}

void rbtree_init(tree_t **root)
{
    if (root != NULL)
        *root = NULL;
}

void rbtree_destroy(tree_t **root)
{
    if (root == NULL || *root == NULL)
        return;

    destroy_rec(*root);
    *root = NULL;
    NIL->parent = NIL;
}
