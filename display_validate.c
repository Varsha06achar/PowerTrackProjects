#include "rbtree.h"
#include <stdio.h>

extern tree_t *rbtree_nil(void);

static void display_rec(tree_t *node, int depth, char branch)
{
    tree_t *nil = rbtree_nil();
    int i;
    if (node == nil) return;
    display_rec(node->right, depth + 1, '/');
    for (i = 0; i < depth; ++i) printf("    ");
    printf("%c-- %d (%c)\n", branch, node->data, node->color == RED ? 'R' : 'B');
    display_rec(node->left, depth + 1, '\\');
}

void display_tree(tree_t **root)
{
    if (root == NULL || *root == NULL) { printf("\nTree is empty.\n"); return; }
    printf("\nTree (R = Red, B = Black):\n");
    display_rec(*root, 0, '*');
}

static int validate_bst(tree_t *node, long long min, long long max)
{
    tree_t *nil = rbtree_nil();
    if (node == nil) return 1;
    if ((long long)node->data <= min || (long long)node->data >= max) return 0;
    if (node->left != nil && node->left->parent != node) return 0;
    if (node->right != nil && node->right->parent != node) return 0;
    return validate_bst(node->left, min, node->data) && validate_bst(node->right, node->data, max);
}

static int black_height(tree_t *node)
{
    int left_height, right_height;
    tree_t *nil = rbtree_nil();
    if (node == nil) return 1;
    if (node->color == RED && (node->left->color == RED || node->right->color == RED)) return -1;
    left_height = black_height(node->left);
    right_height = black_height(node->right);
    if (left_height < 0 || right_height < 0 || left_height != right_height) return -1;
    return left_height + (node->color == BLACK ? 1 : 0);
}

int validate_tree(tree_t **root)
{
    tree_t *nil;
    if (root == NULL) return FAILURE;
    if (*root == NULL) return SUCCESS;
    nil = rbtree_nil();
    if ((*root)->color != BLACK || (*root)->parent != nil) return FAILURE;
    if (!validate_bst(*root, -(long long)2147483649LL, (long long)2147483648LL)) return FAILURE;
    return black_height(*root) >= 0 ? SUCCESS : FAILURE;
}
