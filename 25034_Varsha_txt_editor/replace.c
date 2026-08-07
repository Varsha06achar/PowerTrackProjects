#include "replace.h"

Status replaceText(TextEditor *editor, const char *target, const char *replacement)
{
    int outLine, outPos; 
    
    if(findText(editor, target, &outPos, &outLine) == FAILURE)
        return FAILURE;
    
    if(deleteCharacters(editor, strlen(target)) == FAILURE)
        return FAILURE;
    
    if(insertText(editor, replacement) == FAILURE)
        return FAILURE;
    
    return SUCCESS;
}

Status replaceAll(TextEditor *editor, const char *target, const char *replacement)
{
    int back_up_cursorLine = editor->cursorLine;
    int back_up_cursorPos = editor->cursorPos;
    Node *backup_cursor = editor->cursor;

    editor->cursor = editor->head;
    editor->cursorPos = 0;
    editor->cursorLine = 1;
    
    int total_replacement = 0;
    while(1)
    {
        if(replaceText(editor, target, replacement) == SUCCESS)
            total_replacement++;
        else
            break;
    }

    editor->cursorLine = back_up_cursorLine;
    editor->cursorPos = back_up_cursorPos;
    editor->cursor = backup_cursor;

    printf("Replaced %d Occurences\n", total_replacement);
    return SUCCESS;
}