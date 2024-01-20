#include <stdlib.h>

#include "parser/parser.h"

enum parser_status parse_prefix(struct ast **res, struct lexer *lexer)
{
    if (parse_redirection(res, lexer) == PARSER_OK)
    {
        return PARSER_OK;
    }
    return PARSER_UNEXPECTED_TOKEN;
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
        struct ast *redirect = NULL;
        switch (token)
        {
        case TOKEN_REDIR_STDIN:
            redirect = ast_new(NODE_REDIR_IN, 0, io_number);
            break;
        case TOKEN_REDIR_STDOUT:
            redirect = ast_new(NODE_REDIR_OUT, 0, io_number);
            break;
        case TOKEN_REDIR_APPEND:
            redirect = ast_new(NODE_REDIR_OUTA, 0, io_number);
            break;
        case TOKEN_REDIR_DUPIN:
            redirect = ast_new(NODE_REDIR_INAND, 0, io_number);
            break;
        case TOKEN_REDIR_DUPOUT:
            redirect = ast_new(NODE_REDIR_OUTAND, 0, io_number);
            break;
        case TOKEN_REDIR_INOUT:
            redirect = ast_new(NODE_REDIR_INOUT, 0, io_number);
            break;
        default:
            return PARSER_UNEXPECTED_TOKEN;
        }
        lexer_pop(lexer, true);
        token = lexer_peek(lexer).curr_tok;
        if (token >= TOKEN_IF && token <= TOKEN_WORD)
        {
            char *string = lexer_peek(lexer).tok_str;
            lexer_pop(lexer, false);
            size_t nb_values = 2;
            if (io_number != NULL)
            {
                nb_values++;
            }
            redirect->value =
                realloc(redirect->value, sizeof(char *) * nb_values);
            redirect->value[nb_values - 2] = string;
            redirect->value[nb_values - 1] = NULL;
            *res = redirect;
            return PARSER_OK;
        }
    }
    free(io_number);
    return PARSER_UNEXPECTED_TOKEN;
}
