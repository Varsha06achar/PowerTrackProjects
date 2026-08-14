#include "rbtree.h"

extern tree_t *rbtree_minimum_node(tree_t *node);
extern tree_t *rbtree_maximum_node(tree_t *node);

int find_minimum(tree_t **root, data_t *min)
{
    if (root == NULL || min == NULL || *root == NULL) return FAILURE;
    *min = rbtree_minimum_node(*root)->data;
    return SUCCESS;
}

int find_maximum(tree_t **root, data_t *max)
{
    if (root == NULL || max == NULL || *root == NULL) return FAILURE;
    *max = rbtree_maximum_node(*root)->data;
    return SUCCESS;
}
