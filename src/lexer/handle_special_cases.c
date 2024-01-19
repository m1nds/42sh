#define _POSIX_C_SOURCE 200809L
#include "lexer/handle_special_cases.h"

#include <ctype.h>
#include <string.h>

#include "utils/vector.h"

char skip_blanks(struct lexer *lexer, char c)
{
    while (isblank(c))
    {
        c = fgetc(lexer->input);
    }
    return c;
}

struct lexer_token_save handle_escape(struct lexer *lexer)
{
    struct lexer_token_save out;
    out.tok_str = NULL;
    out.curr_tok = TOKEN_WORD;
    struct vector *vec = vector_create(100);
    char c = fgetc(lexer->input);
    if (c == '\n')
    {
        c = fgetc(lexer->input);
    }
    while (isblank(c) == 0 && c != EOF && c != '\0')
    {
        vector_append(vec, c);
        c = fgetc(lexer->input);
    }
    vector_append(vec, '\0');
    lexer->prev = c;
    out.tok_str = strdup(vec->data);
    vector_destroy(vec);
    return out;
}

struct lexer_token_save handle_double_quote(struct lexer *lexer,
                                            struct vector *vec)
{
    struct lexer_token_save out;
    char c = fgetc(lexer->input);
    while (c != '\"')
    {
        if (c == EOF || c == '\0')
        {
            out.curr_tok = TOKEN_STDIN;
            return out;
        }
        // Handle stuff, like \, $ etc...
        vector_append(vec, c);
        c = fgetc(lexer->input);
    }
    out.curr_tok = TOKEN_WORD;
    return out;
}

struct lexer_token_save handle_single_quote(struct lexer *lexer, char c)
{
    struct lexer_token_save out;
    out.tok_str = NULL;

    struct vector *vec = vector_create(100);
    while (c == '\'')
    {
        c = fgetc(lexer->input);
        while (c != '\'')
        {
            if (c == EOF || c == '\0')
            {
                out.curr_tok = TOKEN_STDIN;
                return out;
            }

            vector_append(vec, c);
            c = fgetc(lexer->input);
        }
        c = fgetc(lexer->input);
    }
    vector_append(vec, '\0');
    lexer->prev = c;
    out.curr_tok = TOKEN_WORD;
    out.tok_str = strdup(vec->data);

    vector_destroy(vec);
    return out;
}

struct lexer_token_save handle_assignment(struct lexer *lexer,
                                          struct vector *vec)
{
    struct lexer_token_save out;
    out.tok_str = NULL;
    out.curr_tok = TOKEN_ASSIGNMENT;
    size_t i = 0;
    while (vec->data[i] != '\0')
    {
        if ((vec->data[i] >= 'a' && vec->data[i] <= 'z')
            || (vec->data[i] >= 'A' && vec->data[i] <= 'Z')
            || (vec->data[i] >= '0' && vec->data[i] <= '9')
            || vec->data[i] == '_')
        {
            i++;
        }
        else
        {
            out.curr_tok = TOKEN_NONE;
            return out;
        }
    }
    out.tok_str = strdup(vec->data);
    vector_destroy(vec);
    lexer->prev = fgetc(lexer->input);
    return out;
}

struct lexer_token_save handle_redirects(struct lexer *lexer, char c)
{
    struct lexer_token_save out;
    if (c == '>')
    {
        c = fgetc(lexer->input);
        if (c == '>')
        {
            out.curr_tok = TOKEN_REDIR_APPEND;
            out.tok_str = strdup(">>");
            lexer->prev = fgetc(lexer->input);
            return out;
        }
        if (c == '&')
        {
            out.curr_tok = TOKEN_REDIR_DUPIN;
            out.tok_str = strdup(">&");
            lexer->prev = fgetc(lexer->input);
            return out;
        }
        if (c == '|')
        {
            c = fgetc(lexer->input);
        }
        out.curr_tok = TOKEN_REDIR_STDOUT;
        out.tok_str = strdup(">");
        lexer->prev = c;
        return out;
    }
    if (c == '<')
    {
        c = fgetc(lexer->input);
        if (c == '&')
        {
            out.curr_tok = TOKEN_REDIR_DUPOUT;
            out.tok_str = strdup("<&");
            lexer->prev = fgetc(lexer->input);
            return out;
        }
        if (c == '>')
        {
            out.curr_tok = TOKEN_REDIR_INOUT;
            out.tok_str = strdup("<>");
            lexer->prev = fgetc(lexer->input);
            return out;
        }
        out.curr_tok = TOKEN_REDIR_STDIN;
        out.tok_str = strdup("<");
        lexer->prev = c;
        return out;
    }
    return out;
}

struct lexer_token_save handle_pipe_or(struct lexer *lexer)
{
    struct lexer_token_save out;
    out.curr_tok = TOKEN_REDIR_PIPE;
    char new_c = fgetc(lexer->input);
    if (new_c == '|')
    {
        out.curr_tok = TOKEN_OR;
        out.tok_str = strdup("||");
        new_c = fgetc(lexer->input);
    }
    else
    {
        out.tok_str = strdup("|");
    }
    lexer->prev = new_c;
    return out;
}

struct lexer_token_save handle_ands(struct lexer *lexer)
{
    struct lexer_token_save out;
    char new_c = fgetc(lexer->input);
    if (new_c == '&')
    {
        out.curr_tok = TOKEN_AND;
        out.tok_str = strdup("&&");
        lexer->prev = fgetc(lexer->input);
    }
    else
    {
        out.curr_tok = TOKEN_NONE;
    }
    return out;
}
