#ifndef CURSOR_NAVIGATIONS_H 
#define CURSOR_NAVIGATIONS_H 

#include "main.h" 

void cursorMoveUp(TextEditor *editor); 
void cursorMoveDown(TextEditor *editor); 
void clampCursorPos(TextEditor *editor); 
void cursorMoveRight(TextEditor *editor); 
void cursorMoveLeft(TextEditor *editor); 
void jumpToStartOfFile(TextEditor *editor);
void jumpToEndOfFile(TextEditor *editor);
void jumpToStartOfLine(TextEditor *editor);
void jumpToEndOfLine(TextEditor *editor);
Status jumpToLine(TextEditor *editor, int targetLine); 

#endif