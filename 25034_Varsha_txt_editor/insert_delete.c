#include "insert_delete.h"

Status insertText(TextEditor *editor, const char *str_input)
{
    if(editor->head == NULL)
    {
        addLine(editor, (char *)str_input);
        editor->cursorPos = strlen(str_input);
        return SUCCESS;
    }

    int old_len = strlen(editor->cursor->text);
    int insert_len = strlen(str_input);
    int new_len = old_len + insert_len;

    char *temp_buff = realloc(editor->cursor->text, (new_len + 1) * sizeof(char));
    if(temp_buff == NULL)
        return FAILURE;
    
    editor->cursor->text = temp_buff;

    //strlen will give you the old_len with null character not counted,
    //So we need to consider it when we are moving the memory
    memmove((editor->cursor->text + editor->cursorPos + insert_len), 
        (editor->cursor->text + editor->cursorPos), 
        (old_len - editor->cursorPos + 1));
    
    //Copy the new string to the cursor text
    strncpy((editor->cursor->text + editor->cursorPos), str_input, insert_len);

    editor->cursorPos = editor->cursorPos + insert_len;

    Action newAction;
    newAction.operation = OP_INSERT;

    /* FIX #2: Check malloc result before using — was missing, could segfault on OOM */
    newAction.text = malloc(sizeof(char) * (insert_len + 1));
    if(newAction.text == NULL)
        return FAILURE;

    strcpy(newAction.text, str_input);
    newAction.cursorLine = editor->cursorLine;
    newAction.cursorPos = editor->cursorPos;

    pushStack(&editor->undoStack, &newAction);
    
    return SUCCESS;
}

Status deleteCharacters(TextEditor *editor, int n)
{
    int old_len = strlen(editor->cursor->text);
    if(old_len == 0)
        return FAILURE;
    
    if(n > editor->cursorPos || n <= 0)
        return FAILURE;
    
    Action newAction;
    newAction.text = malloc(sizeof(char) * (n + 1));
    if(newAction.text == NULL)
        return FAILURE;
    
    strncpy(newAction.text, editor->cursor->text + (editor->cursorPos - n), n);
    newAction.text[n] = '\0';
    newAction.operation = OP_DELETE;
    newAction.cursorLine = editor->cursorLine;

    char *dest = editor->cursor->text + (editor->cursorPos - n);
    char *src = editor->cursor->text + editor->cursorPos;
    int count = old_len - editor->cursorPos + 1;

    memmove(dest, src, count);
    
    editor->cursorPos = editor->cursorPos - n;
    newAction.cursorPos = editor->cursorPos;

    pushStack(&editor->undoStack, &newAction);
    return SUCCESS;
}