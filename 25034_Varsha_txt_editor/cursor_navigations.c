#include "cursor_navigations.h"

void clampCursorPos(TextEditor *editor)
{
    int len_of_new_line = strlen(editor->cursor->text);
    if(editor->cursorPos > len_of_new_line)
    {
        editor->cursorPos = len_of_new_line;
    }
}

void cursorMoveUp(TextEditor *editor)
{
    if(editor->cursor->prev != NULL)
    {
        editor->cursor = editor->cursor->prev;
        editor->cursorLine--;
        clampCursorPos(editor);
    }
}

void cursorMoveDown(TextEditor *editor)
{
    if(editor->cursor->next != NULL)
    {
        editor->cursor = editor->cursor->next;
        editor->cursorLine++;
        clampCursorPos(editor);
    }
}

void cursorMoveRight(TextEditor *editor)
{
    int len = strlen(editor->cursor->text);
    if(editor->cursorPos < len)
    {
        editor->cursorPos++;
    }
    else if(editor->cursorPos == len)
    {
        //If not the end of the line
        if(editor->cursor->next != NULL)
        {
            editor->cursor = editor->cursor->next;
            editor->cursorLine++;
            editor->cursorPos = 0;
        }
    }
}

void cursorMoveLeft(TextEditor *editor)
{
    if(editor->cursorPos > 0)
    {
        editor->cursorPos--;
    }
    else if(editor->cursorPos == 0)
    {
        if(editor->cursor->prev != NULL)
        {
            editor->cursor = editor->cursor->prev;
            editor->cursorLine--;
            editor->cursorPos = strlen(editor->cursor->text);
        }
    }
}

void jumpToStartOfFile(TextEditor *editor)
{
    editor->cursor = editor->head;
    editor->cursorLine = 1;
    editor->cursorPos = 0;
}

void jumpToEndOfFile(TextEditor *editor)
{
    while(editor->cursor->next)
    {
        editor->cursor = editor->cursor->next;
        editor->cursorLine++;
    }
    editor->cursorPos = strlen(editor->cursor->text);
}

void jumpToStartOfLine(TextEditor *editor)
{
    editor->cursorPos = 0;
}

void jumpToEndOfLine(TextEditor *editor)
{
    editor->cursorPos = strlen(editor->cursor->text);
}

Status jumpToLine(TextEditor *editor, int targetLine)
{
    while(targetLine != editor->cursorLine)
    {
        int old_cursorLine = editor->cursorLine;
        if(targetLine < editor->cursorLine)
            cursorMoveUp(editor);
        else
            cursorMoveDown(editor);

        if(old_cursorLine == editor->cursorLine)
            break;
    }

    if(targetLine == editor->cursorLine)
        return SUCCESS;
    else 
        return FAILURE;
}