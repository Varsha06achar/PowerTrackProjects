#ifndef COPY_PASTE_H
#define COPY_PASTE_H

#include "main.h"

ClipboardStack* createStack(int initialCapacity);
Status clipBoardPush(ClipboardStack *stack, char *text);
char* clipBoardPeek(ClipboardStack *stack);
char *clipBoardPeekAt(ClipboardStack *stack, int index); 
void freeClipBoardStack(ClipboardStack *stack);
Status copy(TextEditor *editor, ClipboardStack *cStack, int n);
Status cut(TextEditor *editor, ClipboardStack *cStack, int n);
Status paste(TextEditor *editor, ClipboardStack *cStack);
Status navigateToCopyPoint(TextEditor *editor); 

#endif