#ifndef RBTREE_H
#define RBTREE_H

#include <stddef.h>

#define SUCCESS 1
#define FAILURE 0

typedef int data_t;

typedef enum {
    RED = 0,
    BLACK = 1
} color_t;

typedef struct tree_node {
    data_t data;
    color_t color;
    struct tree_node *left;
    struct tree_node *right;
    struct tree_node *parent;
} tree_t;

void rbtree_init(tree_t **root);
void rbtree_destroy(tree_t **root);

int insert(tree_t **root, data_t item);
int search(tree_t **root, data_t item);

int delete(tree_t **root, data_t item);
int delete_minimum(tree_t **root);
int delete_maximum(tree_t **root);

int find_minimum(tree_t **root, data_t *min);
int find_maximum(tree_t **root, data_t *max);

void display_tree(tree_t **root);
int validate_tree(tree_t **root);

#endif
