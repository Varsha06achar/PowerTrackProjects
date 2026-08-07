#ifndef INSERT_DELETE_H 
#define INSERT_DELETE_H 

#include "main.h" 

Status insertText(TextEditor *editor, const char *str_input); 
Status deleteCharacters(TextEditor *editor, int n); 

#endif