#ifndef STACK_OPS_H 
#define STACK_OPS_H

#include "main.h"

#define CAPACITY 10

Status initDynamicArrayStack(DynamicArrayStack *stack);
Status pushStack(DynamicArrayStack *stack, Action *newAction);
Status popStack(DynamicArrayStack *stack, Action *returnAction);
void freeStack(DynamicArrayStack *stack);

#endif