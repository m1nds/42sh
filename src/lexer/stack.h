#ifndef STACK_H
#define STACH_H

#include <sys/types.h>

struct stack
{
    long value;
    struct stack *next;
};

/*
 * @def: Creates an empty stack and returns it
 */
struct stack *create_stack(void);

/*
 * @def: Pushes the value on to the stack, and returns the new stack
 * @args stack: the stack to push the value onto
 * @args value: the value to push
 */
struct stack *push_stack(struct stack *stack, long value);

/*
 * @def: Pops the first value in the stack and returns it
 * Returns -1 if no value are in the stack
 */
ssize_t pop_stack(struct stack *stack);

/*
 * @def: Frees the stack
 */
void destroy_stack(struct stack *stack);

#endif /* STACK_H */
