#include "stack.h"

#include <stddef.h>
#include <stdlib.h>

struct stack *create_stack(void)
{
    // This looks useless but it might change later
    return NULL;
}

struct stack *push_stack(struct stack *stack, long value)
{
    struct stack *result = calloc(1, sizeof(struct stack));
    if (result == NULL)
    {
        return NULL;
    }
    result->value = value;
    result->next = stack;
    return result;
}

ssize_t pop_stack(struct stack **stack)
{
    struct stack *current = *stack;
    if (current == NULL)
    {
        return -1;
    }
    long result = current->value;
    *stack = current->next;
    free(current);
    return result;
}

void destroy_stack(struct stack *stack)
{
    struct stack *prev = NULL;
    while (stack != NULL)
    {
        prev = stack;
        stack = stack->next;
        free(prev);
    }
}
