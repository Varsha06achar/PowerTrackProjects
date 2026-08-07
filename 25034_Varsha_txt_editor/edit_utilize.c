#include "edit_utilize.h"

void process_full_command(char *full_line, char **command, char **args)
{
    *command = strtok(full_line, " ");
    char *rest = strtok(NULL, "");
    *args = rest;
}

TypesOfCommand processCommand(char *command)
{
    if (command == NULL)
        return C_INVALID;

    if (!strcmp(command, "openfile"))  return C_OPENFILE;
    if (!strcmp(command, "savefile"))  return C_SAVEFILE;
    if (!strcmp(command, "addline"))   return C_ADD_LINE;
    if (!strcmp(command, "insert"))    return C_INSERT;
    if (!strcmp(command, "delete"))    return C_DELETE;
    if (!strcmp(command, "deleteline"))    return C_DELETE_LINE;
    if (!strcmp(command, "search"))    return C_SEARCH;
    if (!strcmp(command, "replace"))   return C_REPLACE;
    if (!strcmp(command, "display"))   return C_PRINT;
    if (!strcmp(command, "copy"))      return C_COPY;
    if (!strcmp(command, "cut"))       return C_CUT;
    if (!strcmp(command, "paste"))     return C_PASTE;
    if (!strcmp(command, "moveup"))    return C_UP;
    if (!strcmp(command, "movedown"))  return C_DOWN;
    if (!strcmp(command, "moveleft"))  return C_LEFT;
    if (!strcmp(command, "moveright")) return C_RIGHT;
    if (!strcmp(command, "jumptoline"))return C_JUMP_TO_LINE;
    if (!strcmp(command, "undo"))      return C_UNDO;
    if (!strcmp(command, "redo"))      return C_REDO;
    if (!strcmp(command, "help"))return C_PRINT_HELP;
    if (!strcmp(command, "exit"))      return C_EXIT;

    return C_INVALID;
}

void printHelp(void) {
    puts("\n================================ HELP =======================================");
    
    puts("\n-- File Operations --");
    puts("openfile <filename> : loads a file into the editor");
    puts("savefile : saves current document to disk");
    puts("exit : quits the editor and frees all memory");

    puts("\n-- Editing --");
    puts("addline <text> : append a new line to the end of the document");
    puts("insert <text> : insert text at the current cursor position");
    puts("delete : deletes N characters BACKWARD from the cursor");
    puts("         (you will be asked how many; cursor at column 0 = nothing to delete)");
    puts("deleteline : if no line number is mentioned, delete the currentline\n");
    puts("          (if line number is mentioned, that line will getdeleted\n)");
    puts("display : shows the full document and current cursor position");

    puts("\n-- Search & Replace --");
    puts("search <text> : search text (prompts for: 1. Find All / 2. Find Next)");
    puts("replace <old> <new> : replace text (prompts for: 1. Replace All / 2. Replace Next)");

    puts("-- Navigation --");
    puts("moveup / movedown / moveleft / moveright : move cursor one step");
    puts("jumptoline <n> : jump cursor directly to line n");
    
    puts("\n-- Clipboard --");
    puts("copy : copies from cursor forward (position cursor first with navigator)");
    puts("cut : copies from cursor forward and deletes text (position cursor first)");
    puts("paste : pastes most recent copy/cut (does not clear clipboard; can paste multiple times)");

    puts("\n-- Undo/Redo --");
    puts("undo : undoes the last insert/delete");
    puts("redo : redoes the last undone action");
    
    puts("=============================================================================\n");
}


void printMenu(void)
{
    printf("\n");
    printf("==========================================================================\n");
    printf("                         SMART TEXT EDITOR\n");
    printf("==========================================================================\n");

    printf("%-40s %-30s\n", "1. Open File      : openfile <filename>", "12. Paste       : paste");
    printf("%-40s %-30s\n", "2. Save File      : savefile",            "13. Move Up     : moveup");
    printf("%-40s %-30s\n", "3. Display        : display",             "14. Move Down   : movedown");
    printf("%-40s %-30s\n", "4. Add Line       : addline <text>",      "15. Move Right  : moveright");
    printf("%-40s %-30s\n", "5. Insert Text    : insert <text>",       "16. Move Left   : moveleft");
    printf("%-40s %-30s\n", "6. Delete Text    : delete",              "17. Jump Line   : jumptoline <n>");
    printf("%-40s %-30s\n", "7. Delete Line    : deleteline",          "18. Undo        : undo");
    printf("%-40s %-30s\n", "8. Search         : search <text>",       "19. Redo        : redo");
    printf("%-40s %-30s\n", "9. Replace        : replace <old> <new>", "20. Help        : help");
    printf("%-40s %-30s\n", "10. Copy          : copy",                "21. Exit        : exit");
    printf("%-40s\n",       "11. Cut           : cut");

    printf("==========================================================================\n");
}

void executeCommand(TextEditor *editor,
                    ClipboardStack *cStack,
                    TypesOfCommand cmd,
                    char *args[],
                    int *running)
{
    switch(cmd)
    {
        case C_ADD_LINE:

            if(*args == NULL)
            {
                printf("[ERROR] No text provided\n");
                break;
            }

            addLine(editor, *args);
            break;

        case C_INSERT:

            if(*args == NULL)
            {
                printf("[ERROR] No text provided\n");
                break;
            }

            insertText(editor, *args);
            break;

        case C_DELETE:
        {
            int n;
            char buf[32];

            printf("Characters to delete : ");

            if(fgets(buf, sizeof(buf), stdin) == NULL)
                break;

            if(sscanf(buf, "%d", &n) != 1)
            {
                printf("Invalid Input\n");
                break;
            }

            deleteCharacters(editor, n);
            break;
        }
        case C_DELETE_LINE:
        {
            if(*args == NULL)
            {
                if(deleteLine(editor, editor->cursorLine) == FAILURE)
                    printf("[INFO] : Delete Line Failed\n");
            }
            else
            {
                int lineNumber = atoi(*args);
                if(deleteLine(editor, lineNumber) == FAILURE)
                    printf("[INFO] : Out of Bound Line Number: %d\n", lineNumber);
            }
            break;
        }
        case C_UNDO:
            undoEdit(editor);
            break;

        case C_REDO:
            redoEdit(editor);
            break;

        case C_SEARCH:
        {
            if(*args == NULL)
            {
                printf("Missing search text\n");
                break;
            }

            char choice[8];

            printf("1.Find All\n");
            printf("2.Find Next\n");
            printf("Choice : ");

            fgets(choice, sizeof(choice), stdin);

            if(choice[0] == '1')
            {
                findAll(editor, *args);
            }
            else if(choice[0] == '2')
            {
                int pos, line;

                if(findText(editor, *args, &pos, &line) == SUCCESS)
                    printf("Found at Line %d Column %d\n", line, pos);
                else
                    printf("Not Found\n");
            }

            break;
        }

        case C_REPLACE:
        {
            char *old_text = strtok(*args, " ");
            if(old_text == NULL)
            {
                printf("replace <old> <new>\n");
                break;
            }    
            char *new_text = strtok(NULL, " ");
            if(new_text == NULL)
            {
                printf("replace <old> <new>\n");
                break;
            }

            char ch;

            printf("1.Replace All\n");
            printf("2.Replace Next\n");

            scanf(" %c", &ch);
            getchar();

            if(ch == '1')
                replaceAll(editor, old_text, new_text);
            else if(ch == '2')
                replaceText(editor, old_text, new_text);

            break;
        }

        case C_PRINT:
            print_list(editor);
            break;

        case C_COPY:
        {
            if(navigateToCopyPoint(editor) == FAILURE)
                break;

            int n;
            char buf[32];
            printf("Enter the number of characters to copy : ");
            if(fgets(buf, sizeof(buf), stdin) == NULL)
                break;
            if(sscanf(buf, "%d", &n) != 1)
            {
                printf("[ERROR] : Invalid number\n");
                break;
            }

            if(copy(editor, cStack, n) == FAILURE)
                printf("[INFO] : Nothing to copy at this position\n");

            break;
        }

        case C_CUT:
        {
            if(navigateToCopyPoint(editor) == FAILURE)
                break;

            int n;
            char buf[32];
            printf("Enter the number of characters to cut : ");
            if(fgets(buf, sizeof(buf), stdin) == NULL)
                break;
            if(sscanf(buf, "%d", &n) != 1)
            {
                printf("[ERROR] : Invalid number\n");
                break;
            }

            if(cut(editor, cStack, n) == FAILURE)
                printf("[INFO] : Nothing to cut at this position\n");

            break;
        }

        case C_PASTE:
        {
            if(navigateToCopyPoint(editor) == FAILURE)
                break;
            if(paste(editor, cStack) == FAILURE)
            {
                printf("[INFO] : Clipboard is empty\n");
                break;
            }

            printf("[INFO] : Text pasted successfully\n");
            break;
        }

        case C_OPENFILE:
        {
            if(*args == NULL)
            {
                printf("[ERROR] : No filename provided\n");
                break;
            }
            if(openFile(editor, *args) == FAILURE)
                printf("[ERROR] : Could not open file \"%s\"\n", *args);
            else
                printf("[INFO] : File \"%s\" loaded successfully\n", *args);
            break;
        }

        case C_SAVEFILE:
        {
            if(saveFile(editor) == FAILURE)
                printf("[ERROR] : Could not save file\n");
            else
                printf("[INFO] : File saved as fileSave.txt\n");
            break;
        }

        case C_UP:
            cursorMoveUp(editor);
            break;
        case C_DOWN:
            cursorMoveDown(editor);
            break;
        case C_LEFT:
            cursorMoveLeft(editor);
            break;
        case C_RIGHT:
            cursorMoveRight(editor);
            break;

        case C_JUMP_TO_LINE:
            if(*args == NULL)
            {
                printf("[INFO] : Missing line number\n");
                break;
            }
            if(jumpToLine(editor, atoi(*args)) == FAILURE)
            {
                printf("[ERROR] : Out of Bounds\n");
            }
            break;

        case C_PRINT_HELP:
            printHelp();
            break;
        case C_EXIT:
            *running = 0;
            break;

        default:
            printf("Invalid Command\n");
    }
}