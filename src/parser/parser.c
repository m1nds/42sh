#include "parser/parser.h"

#include <stdbool.h>
#include <stdlib.h>

#include "lexer/token.h"

enum parser_status parse_input(struct ast **res, struct lexer *lexer)
{
    enum token token = lexer_peek(lexer);
    if (token == TOKEN_RETURN || token == TOKEN_EOF)
    {
        return PARSER_OK;
    }
    if (parse_list(res, lexer) == PARSER_UNEXPECTED_TOKEN)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }
    token = lexer_peek(lexer);
    if (token == TOKEN_RETURN || token == TOKEN_EOF)
    {
        return PARSER_OK;
    }
    return PARSER_UNEXPECTED_TOKEN;
}

enum parser_status parse_list(struct ast **res, struct lexer *lexer)
{
    if (parse_and_or(res, lexer) == PARSER_UNEXPECTED_TOKEN)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }
    enum token token = lexer_peek(lexer);
    while (token == TOKEN_SEMICOLON)
    {
        lexer_pop(lexer);
        if (parse_and_or(res, lexer) == PARSER_UNEXPECTED_TOKEN)
        {
            return PARSER_UNEXPECTED_TOKEN;
        }
        token = lexer_peek(lexer);
    }
    token = lexer_peek(lexer);
    if (token == TOKEN_SEMICOLON)
    {
        lexer_pop(lexer);
    }
    return PARSER_OK;
}

enum parser_status parse_and_or(struct ast **res, struct lexer *lexer)
{
    if (parse_pipeline(res, lexer) == PARSER_UNEXPECTED_TOKEN)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }
    return PARSER_OK;
}

enum parser_status parse_pipeline(struct ast **res, struct lexer *lexer)
{
    if (parse_command(res, lexer) == PARSER_UNEXPECTED_TOKEN)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }
    return PARSER_OK;
}

enum parser_status parse_command(struct ast **res, struct lexer *lexer)
{
    if (parse_simple_command(res, lexer) == PARSER_OK)
    {
        return PARSER_OK;
    }
    if (parse_shell_command(res, lexer) == PARSER_OK)
    {
        return PARSER_OK;
    }
    return PARSER_UNEXPECTED_TOKEN;
}

enum parser_status parse_simple_command(struct ast **res, struct lexer *lexer)
{
    if (parse_element(res, lexer) == PARSER_UNEXPECTED_TOKEN)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }
    struct ast *command = *res;
    size_t loop = 1;
    while (true)
    {
        struct ast *element = NULL;
        if (parse_element(&element, lexer) == PARSER_OK)
        {
            command->value = realloc(command->value, (loop + 2) * sizeof(char *));
            command->value[loop] = element->value[0];
            command->value[loop + 1] = NULL;
        }
        else
        {
            break;
        }
        element->value[0] = NULL;
        ast_free(element);
        loop++;
    }
    *res = command;
    return PARSER_OK;
}

enum parser_status parse_element(struct ast **res, struct lexer *lexer)
{
    enum token token = lexer_peek(lexer);
    if (token == TOKEN_WORD)
    {
        char *string = get_token_string(lexer);
        lexer_pop(lexer);
        // Add token in ast
        struct ast *word = ast_new(NODE_COMMAND, 0, string);
        *res = word;
        return PARSER_OK;
    }
    return PARSER_UNEXPECTED_TOKEN;
}

enum parser_status parse_shell_command(struct ast **res, struct lexer *lexer)
{
    if (parse_rule_if(res, lexer) == PARSER_UNEXPECTED_TOKEN)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }
    return PARSER_OK;
}

enum parser_status parse_rule_if(struct ast **res, struct lexer *lexer)
{
    enum token token = lexer_peek(lexer);
    if (token != TOKEN_IF)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }
    lexer_pop(lexer);
    struct ast *node_if = ast_new(NODE_IF, 3, NULL);
    struct ast *first_child = NULL;
    if (parse_compound_list(&first_child, lexer) == PARSER_UNEXPECTED_TOKEN)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }
    node_if->children[0] = first_child;
    token = lexer_peek(lexer);
    if (token != TOKEN_THEN)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }
    lexer_pop(lexer);
    struct ast *second_child = NULL;
    if (parse_compound_list(&second_child, lexer) == PARSER_UNEXPECTED_TOKEN)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }
    node_if->children[1] = second_child;
    struct ast *third_child = NULL;
    if (parse_else_clause(&third_child, lexer) == PARSER_OK)
    {
        node_if->children[2] = third_child;
    }
    token = lexer_peek(lexer);
    *res = node_if;
    if (token == TOKEN_FI)
    {
        lexer_pop(lexer);
        return PARSER_OK;
    }
    return PARSER_UNEXPECTED_TOKEN;
}

enum parser_status parse_else_clause(struct ast **res, struct lexer *lexer)
{
    enum token token = lexer_peek(lexer);
    if (token == TOKEN_ELSE)
    {
        lexer_pop(lexer);
        return parse_compound_list(res, lexer);
    }
    else if (token == TOKEN_ELIF)
    {
        lexer_pop(lexer);
        struct ast *node_if = ast_new(NODE_IF, 3, NULL);
        struct ast *first_child = NULL;
        if (parse_compound_list(&first_child, lexer) == PARSER_UNEXPECTED_TOKEN)
        {
            return PARSER_UNEXPECTED_TOKEN;
        }
        node_if->children[0] = first_child;
        token = lexer_peek(lexer);
        if (token != TOKEN_THEN)
        {
            return PARSER_UNEXPECTED_TOKEN;
        }
        lexer_pop(lexer);
        struct ast *second_child = NULL;
        if (parse_compound_list(&second_child, lexer) == PARSER_UNEXPECTED_TOKEN)
        {
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

enum parser_status parse_compound_list(struct ast **res, struct lexer *lexer)
{
    if (parse_and_or(res, lexer) == PARSER_UNEXPECTED_TOKEN)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }
    enum token token = lexer_peek(lexer);
    if (token == TOKEN_SEMICOLON)
    {
        lexer_pop(lexer);
        token = lexer_peek(lexer);
    }
    while (token == TOKEN_RETURN)
    {
        lexer_pop(lexer);
        token = lexer_peek(lexer);
    }
    return PARSER_OK;
}
