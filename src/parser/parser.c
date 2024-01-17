#include "parser/parser.h"

#include <stdbool.h>
#include <stdlib.h>

#include "lexer/token.h"

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
        return PARSER_OK;
    }
    if (parse_list(res, lexer) == PARSER_UNEXPECTED_TOKEN)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }
    token = lexer_peek(lexer).curr_tok;
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
    t = lexer_peek(lexer);
    token = t.curr_tok;
    if (token == TOKEN_SEMICOLON)
    {
        lexer_pop(lexer, true);
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
                return PARSER_UNEXPECTED_TOKEN;
            }
            pipe->children = realloc(pipe->children, sizeof(struct ast) * (nb_children + 2));
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
