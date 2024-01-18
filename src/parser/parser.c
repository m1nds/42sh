#include "parser/parser.h"

#include <stdbool.h>
#include <stdlib.h>

#include "lexer/token.h"
#include "parser/list_parser.h"

void print_error(char *message, struct lexer *lexer)
{
    fprintf(stderr, "%s. ", message);
    fprintf(stderr, "Current position is %lu. ", ftell(lexer->input));
    fprintf(stderr, "Current word is %s\n", lexer->ls.tok_str);
}

enum parser_status parse_input(struct ast **res, struct lexer *lexer)
{
    enum token token = lexer_peek(lexer).curr_tok;
    if (token == TOKEN_RETURN || token == TOKEN_EOF)
    {
        if (token == TOKEN_RETURN)
        {
            lexer_pop(lexer, true);
        }
        return PARSER_OK;
    }
    if (parse_list(res, lexer) == PARSER_UNEXPECTED_TOKEN)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }
    token = lexer_peek(lexer).curr_tok;
    if (token == TOKEN_RETURN || token == TOKEN_EOF)
    {
        if (token == TOKEN_RETURN)
        {
            lexer_pop(lexer, true);
        }
        return PARSER_OK;
    }
    return PARSER_UNEXPECTED_TOKEN;
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
    enum token token = lexer_peek(lexer).curr_tok;
    if (token == TOKEN_REDIR_PIPE)
    {
        struct ast *pipe = ast_new(NODE_PIPE, 1, NULL);
        pipe->children[0] = *res;
        size_t nb_children = 1;
        while (token == TOKEN_REDIR_PIPE)
        {
            lexer_pop(lexer, true);
            token = lexer_peek(lexer).curr_tok;
            while (token == TOKEN_RETURN)
            {
                lexer_pop(lexer, true);
                token = lexer_peek(lexer).curr_tok;
            }
            struct ast *child = NULL;
            if (parse_command(&child, lexer) == PARSER_UNEXPECTED_TOKEN)
            {
                ast_free(pipe);
                *res = NULL;
                return PARSER_UNEXPECTED_TOKEN;
            }
            pipe->children =
                realloc(pipe->children, sizeof(struct ast) * (nb_children + 2));
            pipe->children[nb_children] = child;
            nb_children++;
            token = lexer_peek(lexer).curr_tok;
        }
        pipe->children[nb_children + 1] = NULL;
        *res = pipe;
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
    struct lexer_token_save token = lexer_peek(lexer);
    if (token.curr_tok != TOKEN_WORD)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }
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
            command->value =
                realloc(command->value, (loop + 2) * sizeof(char *));
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
    struct lexer_token_save token = lexer_peek(lexer);
    if (token.curr_tok >= TOKEN_IF && token.curr_tok <= TOKEN_WORD)
    {
        char *string = token.tok_str;
        lexer_pop(lexer, false);
        // Add token in ast
        struct ast *word = ast_new(NODE_COMMAND, 0, string);
        *res = word;
        return PARSER_OK;
    }
    return PARSER_UNEXPECTED_TOKEN;
}
