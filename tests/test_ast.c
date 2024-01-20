#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "exec/exec.h"

Test(ast, true_builtin)
{
    struct ast ast = { .node_type = NODE_COMMAND,
                       .value = (char *[]){ strdup("true"), NULL },
                       .children = NULL };

    cr_expect_eq(evaluate_ast(&ast), 0, "Expected: %d. Got: %d", 0,
                 evaluate_ast(&ast));
}

Test(ast, false_builtin)
{
    struct ast ast = { .node_type = NODE_COMMAND,
                       .value = (char *[]){ strdup("false"), NULL },
                       .children = NULL };

    cr_expect_eq(evaluate_ast(&ast), 1, "Expected: %d. Got: %d", 1,
                 evaluate_ast(&ast));
}

Test(ast, double_if)
{
    struct ast echo_a = { .node_type = NODE_COMMAND,
                          .value = (char *[]){ strdup("echo"),
                                               strdup("Hello World!"), NULL },
                          .children = NULL };

    struct ast true_node = { .node_type = NODE_COMMAND,
                             .value = (char *[]){ strdup("true"), NULL },
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

Test(ast, if_but_its_else)
{
    struct ast false_builtin = { .node_type = NODE_COMMAND,
                                 .value = (char *[]){ strdup("false"), NULL },
                                 .children = NULL };

    struct ast echo_true = { .node_type = NODE_COMMAND,
                             .value = (char *[]){ strdup("echo"),
                                                  strdup("true"), NULL },
                             .children = NULL };

    struct ast echo_false = { .node_type = NODE_COMMAND,
                              .value = (char *[]){ strdup("echo"),
                                                   strdup("false"), NULL },
                              .children = NULL };

    struct ast ast = { .node_type = NODE_IF,
                       .value = (char *[]){ strdup("false"), NULL },
                       .children = (struct ast *[]){ &false_builtin, &echo_true,
                                                     &echo_false, NULL } };

    cr_redirect_stdout();

    cr_expect_eq(evaluate_ast(&ast), 0, "Expected: %d. Got: %d", 0,
                 evaluate_ast(&ast));

    fflush(stdout);
    cr_assert_stdout_eq_str("false\n");
}

Test(ast, semicolon_in_if)
{
    struct ast echo_one = { .node_type = NODE_COMMAND,
                            .value = (char *[]){ strdup("echo"), strdup("one"),
                                                 NULL },
                            .children = NULL };

    struct ast echo_two = { .node_type = NODE_COMMAND,
                            .value = (char *[]){ strdup("echo"), strdup("two"),
                                                 NULL },
                            .children = NULL };

    struct ast echo_three = { .node_type = NODE_COMMAND,
                              .value = (char *[]){ strdup("echo"),
                                                   strdup("three"), NULL },
                              .children = NULL };

    struct ast gcc = { .node_type = NODE_COMMAND,
                       .value = (char *[]){ strdup("find"), strdup("-name"),
                                            strdup("test_ast.c"), NULL },
                       .children = NULL };

    struct ast condition_node = { .node_type = NODE_SEMICOLON,
                                  .value = NULL,
                                  .children = (struct ast *[]){ &echo_one, &gcc,
                                                                NULL } };

    struct ast if_two = { .node_type = NODE_IF,
                          .value = NULL,
                          .children = (struct ast *[]){ &condition_node,
                                                        &echo_two, NULL } };

    struct ast if_one = { .node_type = NODE_IF,
                          .value = NULL,
                          .children =
                              (struct ast *[]){ &if_two, &echo_three, NULL } };

    cr_redirect_stdout();

    cr_expect_eq(evaluate_ast(&if_one), 0, "Expected: %d. Got: %d", 0,
                 evaluate_ast(&if_one));

    fflush(stdout);
    cr_assert_stdout_eq_str("one\n./test_ast.c\ntwo\nthree\n");
}
