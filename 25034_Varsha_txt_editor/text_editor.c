#include "text_editor.h"


Status initEditor(TextEditor *editor)
{
    editor->head = NULL;
    editor->tail = NULL;
    editor->cursorLine = 1;
    editor->cursorPos = 0;
    editor->cursor = NULL;

    if(initDynamicArrayStack(&editor->undoStack) == FAILURE)
        return FAILURE;
    
    if(initDynamicArrayStack(&editor->redoStack) == FAILURE)
        return FAILURE;
    
    return SUCCESS;
}

Node* create_node(const char *str)
{
    // 1. allocate memory for a new Node
    Node *new = malloc(sizeof(Node));
    if(new == NULL)
        return NULL;
    
    // 2. allocate memory for the text and copy `str` into it
    int len = strlen(str);

    new->text = malloc((len + 1) * sizeof(char));
    if(new->text == NULL)
    {
        free(new);
        return NULL;
    }

    strcpy(new->text, str);
    // 3. set next and prev to NULL (this new node isn't linked to anything yet)
    new->next = NULL;
    new->prev = NULL;
    // 4. return the pointer to this new node
    return new;
}

Status append_node(TextEditor *editor, Node *new)
{
    if(new == NULL)
        return FAILURE;
    
    if(editor->head == NULL)
    {
        editor->head = new;
        editor->tail = new;
        editor->cursor = editor->head;
        return SUCCESS;
    }

    Node *oldTail = editor->tail;
    new->prev = oldTail;
    oldTail->next = new;
    new->next = NULL;
    editor->tail = new;

    return SUCCESS;
}

Status addLine(TextEditor *editor, char *str)
{
    Node *new = create_node(str);
    if(new == NULL)
        return FAILURE;
    if(append_node(editor, new) == FAILURE)
        return FAILURE;
    return SUCCESS;
}

Status deleteLine(TextEditor *editor, int lineNumber)
{
    if(editor->head == NULL || editor->cursor == NULL)
        return FAILURE;

    if(jumpToLine(editor, lineNumber) == FAILURE)
        return FAILURE;
    
    Node *target = editor->cursor;
    
    // Edge Case 1: It is the ONLY line in the editor
    if (target->prev == NULL && target->next == NULL)
    {
        // Instead of deleting the struct and leaving a NULL head, 
        // flush the text buffer to simulate a fresh empty line.
        free(target->text);
        target->text = malloc(sizeof(char));
        if(target->text == NULL)
            return FAILURE;
            
        target->text[0] = '\0';
        editor->cursorPos = 0;
        return SUCCESS;
    }

    // 1. Route the 'next' pointer of the previous node (or update head)
    if (target->prev != NULL)
        target->prev->next = target->next;
    else
        editor->head = target->next;

    // 2. Route the 'prev' pointer of the next node (or update tail)
    if (target->next != NULL)
        target->next->prev = target->prev;
    else
        editor->tail = target->prev;

    // 3. Safely reassign the cursor before freeing the target
    if (target->next != NULL) 
    {
        // Shifting to the next line. 
        // cursorLine remains the same mathematically because the bottom lines shift up.
        editor->cursor = target->next;
    } 
    else 
    {
        // Target was the tail, so we must fall back to the previous line.
        editor->cursor = target->prev;
        editor->cursorLine--; 
    }
    
    // Reset cursor position to the start of the newly active line
    editor->cursorPos = 0; 

    // 4. Safely free the memory
    free(target->text);
    free(target);
    
    return SUCCESS;
}

void getCurrentLineAndPos(TextEditor *editor)
{
    printf("Cursor Position : Line : %d, Column : %d\n", editor->cursorLine, editor->cursorPos);
}

void print_list(TextEditor *editor)
{
    Node *temp = editor->head;
    int i = 1;
    while(temp)
    {
        printf("Line %d: %s\n", i, temp->text);
        
        if (i == editor->cursorLine)
        {
            // Calculate exactly how many characters the prefix takes up
            char prefix[32];
            int prefix_len = snprintf(prefix, sizeof(prefix), "Line %d: ", i);
            
            // %*s dynamically pads spaces based on the prefix + cursor position
            printf("%*s^\n", prefix_len + editor->cursorPos, "");
        }
        
        temp = temp->next;
        i++;
    }
}

void freeEditor(TextEditor *editor)
{
    while(editor->head)
    {
        Node *temp = editor->head;
        editor->head = editor->head->next;
        free(temp->text);
        free(temp);
    }

    editor->tail = NULL;
    editor->cursor = NULL;   /* FIX #5: was missing — left a dangling pointer */
    editor->cursorLine = 0;
    editor->cursorPos = 0;
}