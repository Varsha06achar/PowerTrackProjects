#ifndef REPLACE_H
#define REPLACE_H

#include "main.h"

Status replaceText(TextEditor *editor, const char *target, const char *replacement);
Status replaceAll(TextEditor *editor, const char *target, const char *replacement);

#endif