#include "main.h"
#include "edit_utilize.h"

int main()
{
    TextEditor editor;

    if(initEditor(&editor) == FAILURE)
        return 1;

    ClipboardStack *cStack = createStack(10);

    if(cStack == NULL)
        return 1;

    char input[256];
    char *command;
    char *args[10];

    int running = 1;

    while(running)
    {
        printMenu();
        getCurrentLineAndPos(&editor);
        printf("Enter Command : ");

        if(fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';

        process_full_command(input, &command, args);

        executeCommand(&editor,
                       cStack,
                       processCommand(command),
                       args,
                       &running);
    }

    freeEditor(&editor);
    freeStack(&editor.undoStack);
    freeStack(&editor.redoStack);
    freeClipBoardStack(cStack); 

    return 0;
}