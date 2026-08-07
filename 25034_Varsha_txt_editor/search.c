#include "search.h"

Status findText(TextEditor *editor, const char *target, int *outPos, int *outLine)
{
    Node *backup_cursor = editor->cursor;
    int backupCursorLine = editor->cursorLine;

    char *result = strstr((editor->cursor->text + editor->cursorPos), target);
    if(result != NULL)
    {
        *outLine = editor->cursorLine;
        *outPos = result - editor->cursor->text;
        jumpToLine(editor, *outLine);
        editor->cursorPos = *outPos + strlen(target);
        return SUCCESS;
    }

    editor->cursor = editor->cursor->next;
    editor->cursorLine++;

    while(editor->cursor)
    {
        result = strstr(editor->cursor->text, target);
        if(result != NULL)
        {
            *outLine = editor->cursorLine;
            *outPos = result - editor->cursor->text;
            jumpToLine(editor, *outLine);
            editor->cursorPos = *outPos + strlen(target);
            return SUCCESS;
        }

        editor->cursor = editor->cursor->next;
        editor->cursorLine++;
    }

    editor->cursor = backup_cursor;
    editor->cursorLine = backupCursorLine;
    
    return FAILURE;
}


void findAll(TextEditor *editor, const char *target)
{
    int outPos, outLine;

    int back_up_cursorLine = editor->cursorLine;
    int back_up_cursorPos = editor->cursorPos;
    Node *backup_cursor = editor->cursor;

    editor->cursorLine = 1;
    editor->cursorPos = 0;

    int found = 0;
    editor->cursor = editor->head;

    while(editor->cursor)
    {
        char *result = strstr((editor->cursor->text + editor->cursorPos), target);
        if(result != NULL)
        {
            outLine = editor->cursorLine;
            outPos = result - editor->cursor->text;
            editor->cursorPos = outPos + strlen(target);
            found++;
            printf("Found \"%s\" at Line : %d, Column : %d\n", target, outLine, outPos);
        }
        else
        {
            editor->cursor = editor->cursor->next;
            editor->cursorLine++;
            editor->cursorPos = 0;
        }
    }

    printf("[SUCCESS] : Search Completed, %d matches Found\n", found);

    editor->cursorLine = back_up_cursorLine;
    editor->cursorPos = back_up_cursorPos;
    editor->cursor = backup_cursor;
}