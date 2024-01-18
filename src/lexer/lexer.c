#define _POSIX_C_SOURCE 200809L
#include "lexer/lexer.h"

#include <stdlib.h>
#include <string.h>

#include "lexer/get_token_type.h"
#include "lexer/handle_special_cases.h"
#include "utils/vector.h"

struct lexer *create_lexer(FILE *input)
{
    struct lexer *result = calloc(1, sizeof(struct lexer));
    // Check for NULL
    if (result == NULL)
    {
        return NULL;
    }
    result->input = input;
    result->pos = 0;
    result->prev = EOF;
    result->ls.curr_tok = TOKEN_NONE;
    // result->stack = create_stack();
    return result;
}

void free_lexer(struct lexer *lexer)
{
    if (lexer != NULL)
    {
        if (lexer->ls.curr_tok != TOKEN_NONE)
        {
            free(lexer->ls.tok_str);
        }
        // destroy_stack(lexer->stack);
        // lexer->stack = NULL;
        free(lexer);
    }
}

/*
 * Returns true if the current character belongs in the word
 * False if not
 */
static bool is_continuous_word(char character)
{
    return character != ' ' && character != ';' && character != '\n'
        && character != '\0' && character != EOF && character != '\t'
        && character != '|' && character != '&' && character != '=';
}

static void ignore_line(struct lexer *lexer)
{
    char c = fgetc(lexer->input);
    while (c != '\n' && c != EOF && c != '\0')
    {
        c = fgetc(lexer->input);
    }
}

static struct lexer_token_save get_part_one(struct lexer *lexer)
{
    char c = lexer->prev;
    if (c == EOF)
    {
        c = fgetc(lexer->input);
    }
    if (c == '\\')
    {
        return handle_escape(lexer, c);
    }
    // Remove all spaces before the word
    c = skip_blanks(lexer, c);
    if (c == '\\')
    {
        return handle_escape(lexer, c);
    }
    if (c == '#')
    {
        ignore_line(lexer);
        c = fgetc(lexer->input);
    }
    struct lexer_token_save out = get_special_character(lexer, c);
    return out;
}

static struct lexer_token_save get_next_token(struct lexer *lexer)
{
    struct lexer_token_save out = get_part_one(lexer);
    if (out.curr_tok != TOKEN_NONE)
    {
        return out;
    }
    char c = lexer->prev;
    struct vector *vec = vector_create(100);
    while (is_continuous_word(c))
    {
        vector_append(vec, c);
        c = fgetc(lexer->input);
    }

    vector_append(vec, '\0');

    out = match_word(vec->data);
    out.tok_str = strdup(vec->data);

    lexer->prev = c;
    vector_destroy(vec);
    return out;
}

struct lexer_token_save lexer_peek(struct lexer *lexer)
{
    if (lexer->ls.curr_tok == TOKEN_NONE)
    {
        lexer->ls = get_next_token(lexer);
    }

    return lexer->ls;
}

void lexer_pop(struct lexer *lexer, bool to_free)
{
    if (lexer->ls.curr_tok == TOKEN_NONE)
    {
        get_next_token(lexer);
        lexer->ls = get_next_token(lexer);
    }
    if (to_free)
    {
        free(lexer->ls.tok_str);
    }
    lexer->ls = get_next_token(lexer);
}

/*void lexer_savestate(struct lexer *lexer)
{
    long value = ftell(lexer->input);
    push_stack(lexer->stack, value);
}

bool lexer_loadstate(struct lexer *lexer)
{
    if (lexer->stack == NULL)
    {
        return false;
    }
    //long value = pop_stack(&(lexer->stack));
    fseek(lexer->input, value, SEEK_SET);
    return true;
}*/
