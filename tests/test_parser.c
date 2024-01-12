#include <criterion/criterion.h>
#include <stdio.h>
#include <string.h>

#include "parser/parser.h"

Test(parser, empty_string)
{
    struct lexer *lexer = create_lexer(fmemopen("", 1, "r+"));
    struct ast *ast = NULL;
    enum parser_status status = parse_input(&ast, lexer);
    cr_expect_eq(status, PARSER_OK, "Expected: OK. Got: PAS_OK.");
    cr_expect_eq(ast, NULL, "Expected empty ast, got ff");
    ast_free(ast);
    free_lexer(lexer);
}

Test(parser, simple_command)
{
    struct lexer *lexer = create_lexer(fmemopen("echo", 1, "r+"));
    struct ast *ast = NULL;
    enum parser_status status = parse_input(&ast, lexer);
    cr_expect_eq(status, PARSER_OK, "Expected: OK. Got: PAS_OK.");
    cr_expect_eq(ast->node_type, NODE_COMMAND, "Expected command, got ff");
    //cr_expect_eq_str(ast->value[0], "echo", "Expected echo, got ff");
    ast_free(ast);
    free_lexer(lexer);
}
