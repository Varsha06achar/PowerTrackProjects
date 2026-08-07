#ifndef TEXT_EDITOR_H 
#define TEXT_EDITOR_H 

#include "main.h" 

Status initEditor(TextEditor *editor);
Node* create_node(const char *str); 
Status append_node(TextEditor *editor, Node *new); 
void print_list(TextEditor *editor); 
Status addLine(TextEditor *editor, char *str); 
Status deleteLine(TextEditor *editor, int lineNumber);
void freeEditor(TextEditor *editor); 
void getCurrentLineAndPos(TextEditor *editor);

#endif