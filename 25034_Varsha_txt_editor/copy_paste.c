#include "copy_paste.h"

ClipboardStack* createStack(int initialCapacity)
{
    ClipboardStack *clipBoardStack = malloc(sizeof(ClipboardStack));
    if(clipBoardStack == NULL)
        return NULL;
    
    clipBoardStack->size = 0;
    clipBoardStack->capacity = initialCapacity;
    clipBoardStack->entries = malloc(sizeof(char *) * clipBoardStack->capacity);
    if(clipBoardStack->entries == NULL)
    {
        free(clipBoardStack);
        return NULL;
    }

    return clipBoardStack;
}


Status clipBoardPush(ClipboardStack *stack, char *text)
{
    if(stack == NULL || text == NULL)
        return FAILURE;
    
    if(stack->size == stack->capacity)
    {
        stack->capacity = 2 * stack->capacity;
        char **temp_entry = (char **)realloc(stack->entries, sizeof(char *) * stack->capacity);
        if(temp_entry == NULL)
            return FAILURE;
        stack->entries = temp_entry;
    }

    stack->entries[stack->size] = (char *)malloc(sizeof(char) * (strlen(text) + 1));
    if(stack->entries[stack->size] == NULL)
        return FAILURE;

    strcpy(stack->entries[stack->size], text);
    stack->size++;

    return SUCCESS;
}

char* clipBoardPeek(ClipboardStack *stack)
{
    if(stack == NULL || stack->size == 0)
        return NULL;
    
    return stack->entries[stack->size - 1];
}

char *clipBoardPeekAt(ClipboardStack *stack, int index)
{
    if(stack->size == 0)
        return NULL;
    
    if(index < 1 || index > stack->size)
        return NULL;
    
    return stack->entries[index - 1];
}

void freeClipBoardStack(ClipboardStack *stack)
{
    if(stack == NULL)
        return;

    for(int i = 0; i < stack->size; i++)
    {
        free(stack->entries[i]);
        stack->entries[i] = NULL;
    }

    free(stack->entries);
    stack->entries = NULL;
    free(stack);          
}

Status copy(TextEditor *editor, ClipboardStack *cStack, int n)
{
    //Only Single Line Copy Available
    int available = (strlen(editor->cursor->text) - editor->cursorPos);
    if(n > available)
    {
        n = available;
    }

    if(n == 0)
    {
        printf("[INFO] : Your Cursor is at the end / Nothing To Copy\n");
        return FAILURE;
    }

    char *copied = malloc(n + 1);
    if(copied == NULL)
        return FAILURE;
    
    strncpy(copied, (editor->cursor->text + editor->cursorPos), n);
    copied[n] = '\0';
    clipBoardPush(cStack, copied);

    printf("[INFO] : Text \"%s\" has been copied to ClipBoard\n", copied);
    free(copied);
    return SUCCESS;
}

Status cut(TextEditor *editor, ClipboardStack *cStack, int n)
{
    if(copy(editor, cStack, n) == FAILURE)
        return FAILURE;
    editor->cursorPos = editor->cursorPos + n;
    if(deleteCharacters(editor, n) == FAILURE)
        return FAILURE;
    
    return SUCCESS;
}

Status paste(TextEditor *editor, ClipboardStack *cStack)
{
    if(cStack->size == 0)
        return FAILURE;
    
    char *pastedText = clipBoardPeek(cStack);
    insertText(editor, pastedText);
    return SUCCESS;
}

/*
 * navigateToCopyPoint
 * -------------------------
 * Interactive mini-navigator that lets the user position the cursor
 * right before issuing copy or cut, without leaving the command loop.
 *
 * Supported sub-commands:
 *   up / down / left / right   - single-step movement
 *   jumptoline <n>             - jump to line n
 *   find <text>                - find next occurrence and move cursor there
 *   done                       - confirm position and proceed (returns SUCCESS)
 *   cancel                     - abort copy/cut (returns FAILURE)
 *
 * Current Line/Col is printed at every prompt so the user always knows
 */
Status navigateToCopyPoint(TextEditor *editor)
{
    char buf[256];

    printf("\n[NAVIGATOR] Move your cursor to the start of what you want to copy/cut.\n");
    printf("  Commands: up | down | left | right | jumptoline <n> | find <text> | addspace\n");
    printf(" jumptoendoffile | jumptostartoffile | jumptostartofline | jumptoendofline | display | done | cancel\n");

    while(1)
    {
        printf("[Line %d, Col %d] nav> ", editor->cursorLine, editor->cursorPos);
        fflush(stdout);

        if(fgets(buf, sizeof(buf), stdin) == NULL)
            return FAILURE;

        buf[strcspn(buf, "\n")] = '\0';

        if(strcmp(buf, "done") == 0)
        {
            printf("[NAVIGATOR] Proceeding from Line %d, Col %d\n",
                   editor->cursorLine, editor->cursorPos);
            return SUCCESS;
        }
        else if(strcmp(buf, "cancel") == 0)
        {
            printf("[NAVIGATOR] Cancelled.\n");
            return FAILURE;
        }
        else if(strcmp(buf, "up") == 0)
            cursorMoveUp(editor);
        else if(strcmp(buf, "down") == 0)
            cursorMoveDown(editor);
        else if(strcmp(buf, "left") == 0)
            cursorMoveLeft(editor);
        else if(strcmp(buf, "right") == 0)
            cursorMoveRight(editor);
        else if(strncmp(buf, "jumptoline ", 11) == 0)
        {
            int line = atoi(buf + 11);
            if(jumpToLine(editor, line) == FAILURE)
                printf("[ERROR] : Line %d is out of bounds\n", line);
        }
        else if(strncmp(buf, "find ", 5) == 0)
        {
            int outPos, outLine;
            if(findText(editor, buf + 5, &outPos, &outLine) == SUCCESS)
            {
                editor->cursorPos = outPos;
                printf("[INFO] : Found \"%s\" — cursor moved to Line %d, Col %d\n",
                       buf + 5, outLine, outPos);
            }    
            else
                printf("[INFO] : \"%s\" not found\n", buf + 5);
        }
        else if(strcmp(buf, "addspace") == 0)
        {
            insertText(editor, " ");
        }
        else if(strcmp(buf, "display") == 0)
        {
            print_list(editor);
        }
        else if(strcmp(buf, "jumptoendoffile") == 0)
        {
            jumpToEndOfFile(editor);
        }
        else if(strcmp(buf, "jumptostartoffile") == 0)
        {
            jumpToStartOfFile(editor);
        }
        else if(strcmp(buf, "jumptostartofline") == 0)
        {
            jumpToStartOfLine(editor);
        }
        else if(strcmp(buf, "jumptoendofline") == 0)
        {
            jumpToEndOfLine(editor);
        }
        else
        {
            printf("[ERROR] : Unknown command. Try: up | down | left | right | jumptoline <n> | find <text> | addspace\n"
                   "jumptoendoffile | jumptostartoffile | jumptostartofline | jumptoendofline | display | done | cancel\n");
        }
    }
}