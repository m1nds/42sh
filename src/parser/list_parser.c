#include "list_parser.h"

#include <stdlib.h>

#include "lexer/token.h"

enum parser_status parse_list(struct ast **res, struct lexer *lexer)
{
    if (parse_and_or(res, lexer) == PARSER_UNEXPECTED_TOKEN)
    {
        print_error("Error while parsing first and_or in list", lexer);
        return PARSER_UNEXPECTED_TOKEN;
    }
    struct lexer_token_save t = lexer_peek(lexer);
    enum token token = t.curr_tok;
    if (token == TOKEN_SEMICOLON)
    {
        size_t nb_children = 0;
        struct ast *semicolon = ast_new(NODE_SEMICOLON, 1, NULL);
        semicolon->children[nb_children] = *res;
        struct ast *child = NULL;
        while (token == TOKEN_SEMICOLON)
        {
            lexer_pop(lexer, true);
            if (parse_and_or(&child, lexer) == PARSER_UNEXPECTED_TOKEN)
            {
                break;
            }
            nb_children++;
            semicolon->children = realloc(
                semicolon->children, sizeof(struct ast) * (nb_children + 1));
            semicolon->children[nb_children] = child;
            child = NULL;
            t = lexer_peek(lexer);
            token = t.curr_tok;
        }
        semicolon->children[nb_children + 1] = NULL;
        *res = semicolon;
    }
    return PARSER_OK;
}

enum parser_status parse_compound_list(struct ast **res, struct lexer *lexer)
{
    enum token token = lexer_peek(lexer).curr_tok;
    while (token == TOKEN_RETURN)
    {
        lexer_pop(lexer, true);
        token = lexer_peek(lexer).curr_tok;
    }
    if (parse_and_or(res, lexer) == PARSER_UNEXPECTED_TOKEN)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }
    token = lexer_peek(lexer).curr_tok;
    if (token == TOKEN_SEMICOLON || token == TOKEN_RETURN)
    {
        size_t nb_children = 0;
        struct ast *semicolon = ast_new(NODE_SEMICOLON, 1, NULL);
        semicolon->children[nb_children] = *res;
        struct ast *child = NULL;
        while (token == TOKEN_SEMICOLON || token == TOKEN_RETURN)
        {
            lexer_pop(lexer, true);
            token = lexer_peek(lexer).curr_tok;
            while (token == TOKEN_RETURN)
            {
                lexer_pop(lexer, true);
                token = lexer_peek(lexer).curr_tok;
            }
            if (parse_and_or(&child, lexer) == PARSER_UNEXPECTED_TOKEN)
            {
                *res = semicolon;
                break;
            }
            nb_children++;
            semicolon->children = realloc(
                semicolon->children, sizeof(struct ast) * (nb_children + 1));
            semicolon->children[nb_children] = child;
            child = NULL;
            token = lexer_peek(lexer).curr_tok;
        }
        semicolon->children[nb_children + 1] = NULL;
        *res = semicolon;
    }
    token = lexer_peek(lexer).curr_tok;
    if (token == TOKEN_SEMICOLON)
    {
        lexer_pop(lexer, true);
        token = lexer_peek(lexer).curr_tok;
    }
    while (token == TOKEN_RETURN)
    {
        lexer_pop(lexer, true);
        token = lexer_peek(lexer).curr_tok;
    }
    return PARSER_OK;
}
