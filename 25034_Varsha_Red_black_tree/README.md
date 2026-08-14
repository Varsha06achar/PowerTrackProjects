# Red Black Tree - Structured C Project

## Structure
```text
red_black_tree_project/
├── include/rbtree.h
├── src/main.c
├── src/insert_search.c
├── src/delete.c
├── src/min_max.c
├── src/display_validate.c
├── src/rbtree_utils.c
├── tests/test_cases.txt
├── Makefile
└── README.md
```

## Modules
- `insert_search.c`: Insert and Search
- `delete.c`: Delete, Delete Minimum, Delete Maximum
- `min_max.c`: Find Minimum and Find Maximum
- `display_validate.c`: Display and Red-Black Tree validation
- `rbtree_utils.c`: shared node, NIL sentinel, rotations, fix-up, transplant and cleanup helpers
- `main.c`: menu-driven application

## Build
```bash
make
```
Run:
```bash
./red_black_tree
```
Clean:
```bash
make clean
```

Manual build:
```bash
gcc -Wall -Wextra -std=c11 -Iinclude src/main.c src/insert_search.c src/delete.c src/min_max.c src/display_validate.c src/rbtree_utils.c -o red_black_tree
```

## Required Operations
1. Insert
2. Delete
3. Search
4. Find Minimum
5. Delete Minimum
6. Find Maximum
7. Delete Maximum

Display and validation are included as supporting operations for testing and demonstration.
