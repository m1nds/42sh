#include "parser.h"

#include <stdbool.h>

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

enum parser_status parse_command(struct ast **res,
                                 struct lexer *lexer) element->value
{
    if (parse_simple_command(res, lexer) == PARSER_UNEXPECTED_TOKEN)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }
    return PARSER_OK;
}

enum parser_status parse_simple_command(struct ast **res, struct lexer *lexer)
{
    if (parse_element(res, lexer) == PARSER_UNEXPECTED_TOKEN)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }
    while (true)
    {
        struct ast *element = NULL;
        size_t loop = 1;
        if (parse_element(&element, lexer) == PARSER_OK)
        {
            *res->value[loop] = element->value[0];
        }
        else
        {
            break;
        }
        ast_free(element);
        loop++;
    }
    return PARSER_OK;
}

enum parser_status parse_element(struct ast **res, struct lexer *lexer)
{
    enum token token = lexer_peek(lexer);
    if (token == TOKEN_WORD)
    {
        lexer_pop(lexer);
        // Add token in ast
        struct ast *word = ast_new(AST_WORD);
        word->*res = word;
        return PARSER_OK;
    }
    return PARSER_UNEXPECTED_TOKEN;
}
