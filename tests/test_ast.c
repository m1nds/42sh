#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"

Test(ast, true)
{
    char *str = calloc(strlen("true") + 1, sizeof(char));
    str = strcpy(str, "true");

    struct ast *ast = ast_new(NODE_COMMAND, 0, str);
    cr_expect_eq(evaluate_ast(ast), 0, "Expected: %d. Got: %d", 0,
                 evaluate_ast(ast));

    ast_free(ast);
}

Test(ast, double_if)
{
    struct ast echo_a = { .node_type = NODE_COMMAND,
                          .value = (char *[]){ "echo", "Hello World!", NULL },
                          .children = NULL };

    struct ast true_node = { .node_type = NODE_COMMAND,
                             .value = (char *[]){ "true", NULL },
                             .children = NULL };

    struct ast if_two = { .node_type = NODE_IF,
                          .value = NULL,
                          .children =
                              (struct ast *[]){ &true_node, &echo_a, NULL } };

    struct ast if_one = { .node_type = NODE_IF,
                          .value = NULL,
                          .children =
                              (struct ast *[]){ &if_two, &echo_a, NULL } };

    cr_redirect_stdout();

    cr_expect_eq(evaluate_ast(&if_one), 0, "Expected: %d. Got: %d", 0,
                 evaluate_ast(&if_one));

    fflush(stdout);
    cr_assert_stdout_eq_str("Hello World!\nHello World!\n");
}
