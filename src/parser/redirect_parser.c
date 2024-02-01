#include <stdlib.h>
#include <string.h>

#include "parser/parser.h"
#include "utils/vector.h"

enum parser_status parse_prefix(struct ast **res, struct lexer *lexer)
{
    enum token token = lexer_peek(lexer).curr_tok;
    if (token == TOKEN_ASSIGNMENT)
    {
        char *name = lexer_peek(lexer).tok_str;
        struct ast *assignment = ast_new(NODE_ASSIGN, 0, name);
        struct vector *vec = vector_create(100);
        lexer_pop(lexer, false);
        token = lexer_peek(lexer).curr_tok;
        while (token >= TOKEN_IF && token <= TOKEN_WORD)
        {
            char *value = lexer_peek(lexer).tok_str;
            vector_append_string(vec, value);
            lexer_pop(lexer, true);
            if (token != TOKEN_ASSIGNMENT)
            {
                break;
            }
            vector_append(vec, '=');
            token = lexer_peek(lexer).curr_tok;
        }
        vector_append(vec, '\0');
        assignment->value = realloc(assignment->value, 3 * sizeof(struct ast));
        assignment->value[1] = strdup(vec->data);
        assignment->value[2] = NULL;
        vector_destroy(vec);
        *res = assignment;
        return PARSER_OK;
    }
    if (parse_redirection(res, lexer) == PARSER_OK)
    {
        return PARSER_OK;
    }
    return PARSER_UNEXPECTED_TOKEN;
}

struct ast *__switch_redirect(enum token token)
{
    switch (token)
    {
    case TOKEN_REDIR_STDIN:
        return ast_new(NODE_REDIR_IN, 0, NULL);
    case TOKEN_REDIR_STDOUT:
        return ast_new(NODE_REDIR_OUT, 0, NULL);
    case TOKEN_REDIR_APPEND:
        return ast_new(NODE_REDIR_OUTA, 0, NULL);
    case TOKEN_REDIR_DUPIN:
        return ast_new(NODE_REDIR_INAND, 0, NULL);
    case TOKEN_REDIR_DUPOUT:
        return ast_new(NODE_REDIR_OUTAND, 0, NULL);
    case TOKEN_REDIR_INOUT:
        return ast_new(NODE_REDIR_INOUT, 0, NULL);
    default:
        return NULL;
    }
    return NULL;
}

enum parser_status parse_redirection(struct ast **res, struct lexer *lexer)
{
    enum token token = lexer_peek(lexer).curr_tok;
    char *io_number = NULL;
    if (token == TOKEN_IO_NUMBER)
    {
        io_number = lexer_peek(lexer).tok_str;
        lexer_pop(lexer, false);
        token = lexer_peek(lexer).curr_tok;
    }
    if (token >= TOKEN_REDIR_STDIN && token <= TOKEN_REDIR_INOUT)
    {
        struct ast *redirect = __switch_redirect(token);
        if (redirect == NULL)
        {
            free(io_number);
            return PARSER_UNEXPECTED_TOKEN;
        }
        lexer_pop(lexer, true);
        token = lexer_peek(lexer).curr_tok;
        if (token >= TOKEN_IF && token <= TOKEN_WORD)
        {
            char *string = lexer_peek(lexer).tok_str;
            lexer_pop(lexer, false);
            redirect->value = realloc(redirect->value, sizeof(char *) * 3);
            redirect->value[0] = string;
            redirect->value[1] = io_number;
            redirect->value[2] = NULL;
            *res = redirect;
            return PARSER_OK;
        }
        ast_free(redirect);
        free(io_number);
        return PARSER_REALLY_NOT_OK;
    }
    free(io_number);
    return PARSER_UNEXPECTED_TOKEN;
}
