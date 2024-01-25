#include "shell_parser.h"

#include <stdlib.h>

#include "parser.h"

enum parser_status parse_shell_command(struct ast **res, struct lexer *lexer)
{
    enum token token = lexer_peek(lexer).curr_tok;
    if (token == TOKEN_LEFT_BRACKET)
    {
        lexer_pop(lexer, true);
        if (parse_compound_list(res, lexer) == PARSER_UNEXPECTED_TOKEN)
        {
            return PARSER_UNEXPECTED_TOKEN;
        }
        token = lexer_peek(lexer).curr_tok;
        if (token != TOKEN_RIGHT_BRACKET)
        {
            return PARSER_UNEXPECTED_TOKEN;
        }
        lexer_pop(lexer, true);
        return PARSER_OK;
    }
    if (parse_rule_if(res, lexer) == PARSER_UNEXPECTED_TOKEN
        && parse_rule_while(res, lexer) == PARSER_UNEXPECTED_TOKEN
        && parse_rule_until(res, lexer) == PARSER_UNEXPECTED_TOKEN)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }
    return PARSER_OK;
}

enum parser_status parse_rule_while(struct ast **res, struct lexer *lexer)
{
    struct lexer_token_save t = lexer_peek(lexer);
    enum token token = t.curr_tok;
    if (token != TOKEN_WHILE)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }
    lexer_pop(lexer, true);
    struct ast *node_while = ast_new(NODE_WHILE, 2, NULL);
    struct ast *first_child = NULL;
    if (parse_compound_list(&first_child, lexer) == PARSER_UNEXPECTED_TOKEN)
    {
        ast_free(node_while);
        *res = NULL;
        print_error("Error while parsing first child of while", lexer);
        return PARSER_UNEXPECTED_TOKEN;
    }
    node_while->children[0] = first_child;
    t = lexer_peek(lexer);
    token = t.curr_tok;
    if (token != TOKEN_DO)
    {
        ast_free(node_while);
        *res = NULL;
        print_error("Error while parsing do", lexer);
        return PARSER_UNEXPECTED_TOKEN;
    }
    lexer_pop(lexer, true);
    struct ast *second_child = NULL;
    if (parse_compound_list(&second_child, lexer) == PARSER_UNEXPECTED_TOKEN)
    {
        ast_free(node_while);
        *res = NULL;
        print_error("Error while parsing second child of while", lexer);
        return PARSER_UNEXPECTED_TOKEN;
    }
    node_while->children[1] = second_child;
    t = lexer_peek(lexer);
    token = t.curr_tok;
    *res = node_while;
    if (token == TOKEN_DONE)
    {
        lexer_pop(lexer, true);
        return PARSER_OK;
    }
    return PARSER_UNEXPECTED_TOKEN;
}

enum parser_status parse_rule_until(struct ast **res, struct lexer *lexer)
{
    struct lexer_token_save t = lexer_peek(lexer);
    enum token token = t.curr_tok;
    if (token != TOKEN_UNTIL)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }
    lexer_pop(lexer, true);
    struct ast *node_until = ast_new(NODE_UNTIL, 2, NULL);
    struct ast *first_child = NULL;
    if (parse_compound_list(&first_child, lexer) == PARSER_UNEXPECTED_TOKEN)
    {
        ast_free(node_until);
        *res = NULL;
        print_error("Error while parsing first child of until", lexer);
        return PARSER_UNEXPECTED_TOKEN;
    }
    node_until->children[0] = first_child;
    t = lexer_peek(lexer);
    token = t.curr_tok;
    if (token != TOKEN_DO)
    {
        ast_free(node_until);
        *res = NULL;
        print_error("Error while parsing do", lexer);
        return PARSER_UNEXPECTED_TOKEN;
    }
    lexer_pop(lexer, true);
    struct ast *second_child = NULL;
    if (parse_compound_list(&second_child, lexer) == PARSER_UNEXPECTED_TOKEN)
    {
        ast_free(node_until);
        *res = NULL;
        print_error("Error while parsing second child of until", lexer);
        return PARSER_UNEXPECTED_TOKEN;
    }
    node_until->children[1] = second_child;
    t = lexer_peek(lexer);
    token = t.curr_tok;
    *res = node_until;
    if (token == TOKEN_DONE)
    {
        lexer_pop(lexer, true);
        return PARSER_OK;
    }
    return PARSER_UNEXPECTED_TOKEN;
}
enum parser_status parse_rule_if(struct ast **res, struct lexer *lexer)
{
    struct lexer_token_save t = lexer_peek(lexer);
    enum token token = t.curr_tok;
    if (token != TOKEN_IF)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }
    lexer_pop(lexer, true);
    struct ast *node_if = ast_new(NODE_IF, 3, NULL);
    struct ast *first_child = NULL;
    if (parse_compound_list(&first_child, lexer) == PARSER_UNEXPECTED_TOKEN)
    {
        ast_free(node_if);
        *res = NULL;
        print_error("Error while parsing first child of if", lexer);
        return PARSER_UNEXPECTED_TOKEN;
    }
    node_if->children[0] = first_child;
    t = lexer_peek(lexer);
    token = t.curr_tok;
    if (token != TOKEN_THEN)
    {
        ast_free(node_if);
        *res = NULL;
        print_error("Error while parsing then", lexer);
        return PARSER_UNEXPECTED_TOKEN;
    }
    lexer_pop(lexer, true);
    struct ast *second_child = NULL;
    if (parse_compound_list(&second_child, lexer) == PARSER_UNEXPECTED_TOKEN)
    {
        ast_free(node_if);
        *res = NULL;
        print_error("Error while parsing second child of if", lexer);
        return PARSER_UNEXPECTED_TOKEN;
    }
    node_if->children[1] = second_child;
    struct ast *third_child = NULL;
    if (parse_else_clause(&third_child, lexer) == PARSER_OK)
    {
        node_if->children[2] = third_child;
    }
    t = lexer_peek(lexer);
    token = t.curr_tok;
    *res = node_if;
    if (token == TOKEN_FI)
    {
        lexer_pop(lexer, true);
        return PARSER_OK;
    }
    return PARSER_UNEXPECTED_TOKEN;
}

enum parser_status parse_else_clause(struct ast **res, struct lexer *lexer)
{
    struct lexer_token_save t = lexer_peek(lexer);
    enum token token = t.curr_tok;
    if (token == TOKEN_ELSE)
    {
        lexer_pop(lexer, true);
        return parse_compound_list(res, lexer);
    }
    else if (token == TOKEN_ELIF)
    {
        lexer_pop(lexer, true);
        struct ast *node_if = ast_new(NODE_IF, 3, NULL);
        struct ast *first_child = NULL;
        if (parse_compound_list(&first_child, lexer) == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free(node_if);
            *res = NULL;
            print_error("Error while parsing first child of elif", lexer);
            return PARSER_UNEXPECTED_TOKEN;
        }
        node_if->children[0] = first_child;
        t = lexer_peek(lexer);
        token = t.curr_tok;
        if (token != TOKEN_THEN)
        {
            ast_free(node_if);
            *res = NULL;
            print_error("Error while parsing then of elif", lexer);
            return PARSER_UNEXPECTED_TOKEN;
        }
        lexer_pop(lexer, true);
        struct ast *second_child = NULL;
        if (parse_compound_list(&second_child, lexer)
            == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free(node_if);
            *res = NULL;
            print_error("Error while parsing second child of elif", lexer);
            return PARSER_UNEXPECTED_TOKEN;
        }
        node_if->children[1] = second_child;
        struct ast *third_child = NULL;
        if (parse_else_clause(&third_child, lexer) == PARSER_OK)
        {
            node_if->children[2] = third_child;
        }
        *res = node_if;
        return PARSER_OK;
    }
    else
    {
        return PARSER_UNEXPECTED_TOKEN;
    }
}

enum parser_status parse_funcdec(struct ast **res, struct lexer *lexer)
{
    struct lexer_token_save t = lexer_peek(lexer);
    enum token token = t.curr_tok;
    if (token != TOKEN_WORD && TOKEN_ESCAPED_WORD)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }
    struct ast *function = ast_new(NODE_FUNCTION, 1, t.tok_str);
    lexer_pop(lexer, false);
    token = lexer_peek(lexer).curr_tok;
    if (token != TOKEN_LEFT_PARENTHESES)
    {
        ast_free(function);
        return PARSER_UNEXPECTED_TOKEN;
    }
    lexer_pop(lexer, true);
    token = lexer_peek(lexer).curr_tok;
    if (token != TOKEN_RIGHT_PARENTHESES)
    {
        ast_free(function);
        return PARSER_UNEXPECTED_TOKEN;
    }
    lexer_pop(lexer, true);
    token = lexer_peek(lexer).curr_tok;
    while (token == TOKEN_RETURN)
    {
        lexer_pop(lexer, true);
        token = lexer_peek(lexer).curr_tok;
    }
    struct ast *shell_command = NULL;
    if (parse_shell_command(&shell_command, lexer) == PARSER_UNEXPECTED_TOKEN)
    {
        ast_free(function);
        return PARSER_UNEXPECTED_TOKEN;
    }
    function->children[0] = shell_command;
    *res = function;
    return PARSER_OK;
}
