#include <criterion/criterion.h>
#include <stdio.h>
#include <string.h>

#include "lexer/stack.h"

Test(stack, simple_test)
{
    struct stack *stack = create_stack();
    stack = push_stack(stack, 20);
    stack = push_stack(stack, 30);
    cr_expect_eq(pop_stack(&stack), 30);
    cr_expect_eq(pop_stack(&stack), 20);
    cr_expect_eq(pop_stack(&stack), -1);

    destroy_stack(stack);
}
