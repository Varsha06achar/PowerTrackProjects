#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 1. DEFINE ALL TYPES AND STRUCTURES FIRST */
typedef enum {
    SUCCESS,
    FAILURE
} Status;

typedef enum {
    OP_INSERT,
    OP_DELETE
} OperationType;


typedef struct {
    char **entries;
    int size;
    int capacity;
} ClipboardStack;

typedef struct {
    OperationType operation;  // "insert" or "delete"
    char *text;               // the text involved
    int cursorLine;
    int cursorPos;
} Action;

typedef struct {
    Action *actions;
    int size;       // how many actions currently stored
    int capacity;   // how many actions the array can hold before needing to grow
} DynamicArrayStack;

// Represents ONE line of text  like one train car
typedef struct Node {
    char *text;         // the actual line content, e.g. "Hello World"
    struct Node *next;  // pointer to the line after this one
    struct Node *prev;  // pointer to the line before this one
} Node;

// Represents the WHOLE document  like the train itself
typedef struct {
    DynamicArrayStack undoStack;
    DynamicArrayStack redoStack;
    // ClipboardStack clipBoard;
    Node *head;    // pointer to the FIRST line node
    Node *tail;    // pointer to the LAST line node
    Node *cursor;  // pointer to whichever line the cursor is on
    int cursorLine;
    int cursorPos;
} TextEditor;


/* 2. INCLUDE SUB-MODULES AFTER TYPES ARE DEFINED */
#include "text_editor.h"
#include "cursor_navigations.h"
#include "insert_delete.h"
#include "redo_undo.h"
#include "file_ops.h"
#include "stack_ops.h"
#include "search.h"
#include "replace.h"
#include "copy_paste.h"
#include "edit_utilize.h"

#endif