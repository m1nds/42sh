#include "parser/parser.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/token.h"
#include "parser/list_parser.h"
#include "utils/vector.h"

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
    enum token token = lexer_peek(lexer).curr_tok;
    while (token == TOKEN_AND || token == TOKEN_OR)
    {
        struct ast *node = NULL;
        struct ast *child = NULL;
        switch (token)
        {
        case TOKEN_AND:
            node = ast_new(NODE_AND, 2, NULL);
            node->children[0] = *res;
            break;
        case TOKEN_OR:
            node = ast_new(NODE_OR, 2, NULL);
            node->children[0] = *res;
            break;
        default:
            break;
        }
        lexer_pop(lexer, true);
        token = lexer_peek(lexer).curr_tok;
        while (token == TOKEN_RETURN)
        {
            lexer_pop(lexer, true);
            token = lexer_peek(lexer).curr_tok;
        }
        if (parse_pipeline(&child, lexer) == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free(node);
            *res = NULL;
            return PARSER_UNEXPECTED_TOKEN;
        }
        node->children[1] = child;
        token = lexer_peek(lexer).curr_tok;
        *res = node;
    }
    return PARSER_OK;
}

static enum parser_status __parse_pipeline_loop(struct ast **res,
                                                struct lexer *lexer)
{
    enum token token = lexer_peek(lexer).curr_tok;
    if (token == TOKEN_REDIR_PIPE)
    {
        struct ast *pipe = ast_new(NODE_PIPE, 1, NULL);
        if ((*res)->node_type == NODE_NOT)
        {
            pipe->children[0] = (*res)->children[0];
            (*res)->children[0] = pipe;
        }
        else
        {
            pipe->children[0] = *res;
            *res = pipe;
        }
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
                ast_free(*res);
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
    }
    return PARSER_OK;
}

enum parser_status parse_pipeline(struct ast **res, struct lexer *lexer)
{
    enum token not_tk = lexer_peek(lexer).curr_tok;
    if (not_tk == TOKEN_NOT)
    {
        struct ast *not_ast = ast_new(NODE_NOT, 1, NULL);
        struct ast *child = NULL;
        lexer_pop(lexer, true);
        if (parse_command(&child, lexer) == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free(not_ast);
            return PARSER_UNEXPECTED_TOKEN;
        }
        not_ast->children[0] = child;
        *res = not_ast;
    }
    else
    {
        if (parse_command(res, lexer) == PARSER_UNEXPECTED_TOKEN)
        {
            return PARSER_UNEXPECTED_TOKEN;
        }
    }
    return __parse_pipeline_loop(res, lexer);
}

enum parser_status __parse_redirects(struct ast **res, struct lexer *lexer)
{
    struct ast *redirect = NULL;
    if (parse_redirection(&redirect, lexer) == PARSER_OK)
    {
        struct ast *parent = NULL;
        if ((*res)->node_type == NODE_FUNCTION)
        {
            parent = ast_new(NODE_FUNCTION, 1, (*res)->value[0]);
            parent->children[0] = ast_new(NODE_REDIR, 2, NULL);
            parent->children[0]->children[0] = (*res)->children[0];
            parent->children[0]->children[1] = redirect;
        }
        else
        {
            parent = ast_new(NODE_REDIR, 2, NULL);
            parent->children[0] = *res;
            parent->children[1] = redirect;
        }
        size_t nb_children = 2;
        redirect = NULL;
        while (parse_redirection(&redirect, lexer) == PARSER_OK)
        {
            if (parent->node_type == NODE_REDIR)
            {
                parent->children = realloc(
                    parent->children, sizeof(struct ast) * (nb_children + 2));
                parent->children[nb_children] = redirect;
            }
            else
            {
                parent->children[0]->children = realloc(
                    parent->children, sizeof(struct ast) * (nb_children + 2));
                parent->children[0]->children[nb_children] = redirect;
            }
            redirect = NULL;
            nb_children++;
        }
        parent->children[nb_children + 1] = NULL;
        *res = parent;
    }
    return PARSER_OK;
}

enum parser_status parse_command(struct ast **res, struct lexer *lexer)
{
    enum token token = lexer_next_peek(lexer).curr_tok;
    if (token == TOKEN_LEFT_PARENTHESES)
    {
        if (parse_funcdec(res, lexer) == PARSER_OK)
        {
            return __parse_redirects(res, lexer);
        }
    }
    if (parse_simple_command(res, lexer) == PARSER_OK)
    {
        return PARSER_OK;
    }
    if (parse_shell_command(res, lexer) == PARSER_OK)
    {
        return __parse_redirects(res, lexer);
    }
    return PARSER_UNEXPECTED_TOKEN;
}

static enum parser_status __parse_element_loop(struct ast **res,
                                               struct ast *result,
                                               size_t nb_children,
                                               struct lexer *lexer)
{
    struct ast *command = *res;
    size_t loop = 1;
    while (true)
    {
        struct ast *element = NULL;
        enum parser_status status = parse_element(&element, lexer);
        if (status == PARSER_OK)
        {
            if (element->node_type == NODE_COMMAND)
            {
                command->value =
                    realloc(command->value, (loop + 2) * sizeof(char *));
                command->value[loop] = element->value[0];
                command->value[loop + 1] = NULL;
                element->value[0] = NULL;
                ast_free(element);
                loop++;
            }
            else
            {
                result->children = realloc(
                    result->children, sizeof(struct ast) * (nb_children + 2));
                result->children[nb_children] = element;
                nb_children++;
            }
        }
        else if (status == PARSER_UNEXPECTED_TOKEN)
        {
            break;
        }
        else
        {
            ast_free(result);
            ast_free(command);
            *res = NULL;
            return PARSER_UNEXPECTED_TOKEN;
        }
    }
    result->children =
        realloc(result->children, sizeof(struct ast) * (nb_children + 2));
    result->children[nb_children] = command;
    result->children[nb_children + 1] = NULL;
    *res = result;
    return PARSER_OK;
}

enum parser_status parse_simple_command(struct ast **res, struct lexer *lexer)
{
    struct ast *result = ast_new(NODE_REDIR, 1, NULL);
    size_t nb_children = 0;
    char flag = 0;
    if (parse_prefix(res, lexer) == PARSER_OK)
    {
        flag = 1;
        result->children[0] = *res;
        nb_children++;
        struct ast *child = NULL;
        while (parse_prefix(&child, lexer) == PARSER_OK)
        {
            result->children = realloc(result->children,
                                       sizeof(struct ast) * (nb_children + 2));
            result->children[nb_children] = child;
            nb_children++;
            child = NULL;
        }
        result->children[nb_children + 1] = NULL;
    }
    struct lexer_token_save token = lexer_peek(lexer);
    if (token.curr_tok != TOKEN_WORD && token.curr_tok != TOKEN_ESCAPED_WORD)
    {
        if (flag == 1)
        {
            *res = result;
            return PARSER_OK;
        }
        ast_free(result);
        return PARSER_UNEXPECTED_TOKEN;
    }
    if (parse_element(res, lexer) == PARSER_UNEXPECTED_TOKEN)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }
    return __parse_element_loop(res, result, nb_children, lexer);
}

enum parser_status parse_element(struct ast **res, struct lexer *lexer)
{
    struct lexer_token_save token = lexer_peek(lexer);
    if (token.curr_tok >= TOKEN_IF && token.curr_tok <= TOKEN_WORD)
    {
        struct vector *vec = vector_create(100);
        while (token.curr_tok >= TOKEN_IF && token.curr_tok <= TOKEN_WORD)
        {
            char *string = token.tok_str;
            vector_append_string(vec, string);
            lexer_pop(lexer, true);
            if (token.curr_tok != TOKEN_ASSIGNMENT)
            {
                break;
            }
            vector_append(vec, '=');
            token = lexer_peek(lexer);
        }
        // Add token in ast
        vector_append(vec, '\0');
        char *str = strdup(vec->data);
        vector_destroy(vec);
        struct ast *word = ast_new(NODE_COMMAND, 0, str);
        *res = word;
        return PARSER_OK;
    }
    enum parser_status status = parse_redirection(res, lexer);
    return status;
}
