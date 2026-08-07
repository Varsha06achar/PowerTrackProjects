#include "stack_ops.h"

Status initDynamicArrayStack(DynamicArrayStack *stack)
{
    stack->size = 0;
    stack->capacity = CAPACITY;
    stack->actions = malloc(sizeof(Action) * stack->capacity);
    if(stack->actions == NULL)
    {
        return FAILURE;
    }

    return SUCCESS;
}

Status pushStack(DynamicArrayStack *stack, Action *newAction)
{
    if(stack->size == stack->capacity)
    {
        stack->capacity *= 2;
        Action *temp = realloc(stack->actions, sizeof(Action) * stack->capacity);
        if(temp == NULL)
            return FAILURE;
        
        stack->actions = temp;
    }

    stack->actions[stack->size] = *newAction;
    stack->size++;
    return SUCCESS;
}

Status popStack(DynamicArrayStack *stack, Action *returnAction)
{
    if(stack->size == 0)
        return FAILURE;
    
    *returnAction = stack->actions[stack->size - 1];
    stack->size--;
    return SUCCESS;
}

void freeStack(DynamicArrayStack *stack)
{
    for(int i = 0; i < stack->size; i++)
    {
        free(stack->actions[i].text);
        stack->actions[i].text = NULL;
    }
    free(stack->actions);
    stack->actions = NULL;
}