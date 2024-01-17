#define _POSIX_C_SOURCE 200809L
#include "handle_special_cases.h"

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

struct lexer_token_save handle_escape(struct lexer *lexer, char c)
{
    struct lexer_token_save out;
    out.tok_str = NULL;
    out.curr_tok = TOKEN_WORD;

    struct vector *vec = vector_create(100);
    vector_append(vec, c);
    c = fgetc(lexer->input);
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

struct lexer_token_save handle_single_quote(struct lexer *lexer)
{
    struct lexer_token_save out;
    out.tok_str = NULL;

    char c = fgetc(lexer->input);
    struct vector *vec = vector_create(100);

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
    vector_append(vec, '\0');
    lexer->prev = c;
    out.curr_tok = TOKEN_WORD;
    out.tok_str = strdup(vec->data);

    vector_destroy(vec);
    return out;
}
