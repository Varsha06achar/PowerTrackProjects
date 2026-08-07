#ifndef SEARCH_H
#define SEARCH_H

#include "main.h"

Status findText(TextEditor *editor, const char *target, int *outPos, int *outLine);
void findAll(TextEditor *editor, const char *target);

#endif