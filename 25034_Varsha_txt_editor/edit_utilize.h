#ifndef EDIT_UTILIZE_H
#define EDIT_UTILIZE_H

#include "main.h"

typedef enum
{
    C_ADD_LINE,
    C_INSERT,
    C_DELETE,
    C_DELETE_LINE,
    C_UNDO,
    C_REDO,
    C_SEARCH,
    C_REPLACE,
    C_PRINT,
    C_COPY,
    C_CUT,
    C_PASTE,
    C_OPENFILE,
    C_SAVEFILE,
    C_UP,
    C_DOWN,
    C_LEFT,
    C_RIGHT,
    C_JUMP_TO_LINE,
    C_INVALID,
    C_PRINT_HELP,
    C_EXIT
} TypesOfCommand;

void process_full_command(char *full_line, char **command, char **args);

TypesOfCommand processCommand(char *command);

void executeCommand(TextEditor *editor,
                    ClipboardStack *cStack,
                    TypesOfCommand cmd,
                    char *args[],
                    int *running);

void printMenu(void);
void printHelp(void);

#endif