#define _POSIX_C_SOURCE 200809L
#include "lexer/lexer.h"

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

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
    //result->stack = create_stack();
    return result;
}

void free_lexer(struct lexer *lexer)
{
    if (lexer != NULL)
    {
        //destroy_stack(lexer->stack);
        //lexer->stack = NULL;
        free(lexer);
    }
}

static struct lexer_token_save match_word(char *word)
{
    struct lexer_token_save out;
    out.curr_tok = TOKEN_WORD;

    if (strcmp(word, "if") == 0)
    {
        out.curr_tok = TOKEN_IF;
        return out;
    }
    if (strcmp(word, "then") == 0)
    {
        out.curr_tok = TOKEN_THEN;
        return out;
    }
    if (strcmp(word, "elif") == 0)
    {
        out.curr_tok = TOKEN_ELIF;
        return out;
    }
    if (strcmp(word, "else") == 0)
    {
        out.curr_tok = TOKEN_ELSE;
        return out;
    }
    if (strcmp(word, "fi") == 0)
    {
        out.curr_tok = TOKEN_FI;
        return out;
    }

    return out;
}

/*
 * Returns true if the current character belongs in the word
 * False if not
 */
static bool is_continuous_word(char character)
{
    return character != ' ' && character != ';' && character != '\n'
        && character != '\0' && character != EOF;
}

static void ignore_line(struct lexer *lexer)
{
    char c = fgetc(lexer->input);
    while (c != '\n' && c != EOF && c != '\0')
    {
        c = fgetc(lexer->input);
    }
}

static struct lexer_token_save check_single_quote(struct lexer *lexer)
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

static char skip_blanks(struct lexer *lexer, char c)
{
    while (isblank(c))
    {
        c = fgetc(lexer->input);
    }

    return c;
}

static struct lexer_token_save get_next_token(struct lexer *lexer)
{
    char c = lexer->prev;
    if (c == EOF)
    {
        c = fgetc(lexer->input);
    }
    // Remove all spaces before the word
    c = skip_blanks(lexer, c);

    if (c == '#')
    {
        ignore_line(lexer);
        c = fgetc(lexer->input);
    }

    struct lexer_token_save out;
    out.tok_str = NULL;
    switch (c)
    {
    case EOF:
    case '\0':
        out.curr_tok = TOKEN_EOF;
        return out;
    case ';':
        out.curr_tok = TOKEN_SEMICOLON;
        out.tok_str = strdup(";");
        lexer->prev = fgetc(lexer->input);
        return out;
    case '\'':
        return check_single_quote(lexer);
    case '\\':
        c = fgetc(lexer->input);
        break;
    case '\n':
        lexer->prev = fgetc(lexer->input);
        out.curr_tok = TOKEN_RETURN;
        out.tok_str = strdup("\n");
        return out;
    }

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

/*static void skip_single_quote(struct lexer *lexer)
{
    lexer->pos++;
    char c = fgetc(lexer->input);
    while (c != '\'')
    {
        c = fgetc(lexer->input);
        lexer->pos++;
    }
    lexer->pos++;
}*/

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

/*static char *handle_single_quote(struct lexer *lexer, char *buffer)
{
    size_t buffer_pos = 0;
    char c = fgetc(lexer->input);
    while (c != '\'')
    {
        buffer[buffer_pos++] = c;
        c = fgetc(lexer->input);
        if (c == EOF || c == '\0')
        {
            free(buffer);
            return "no end to single quote";
        }
        if (buffer_pos % 100 == 0)
        {
            buffer = realloc(buffer, (buffer_pos + 100) * sizeof(char));
        }
    }
    buffer[buffer_pos] = '\0';
    fseek(lexer->input, lexer->pos, SEEK_SET);
    return buffer;
}

char *get_token_string(struct lexer *lexer)
{
    enum token token = lexer_peek(lexer);
    if (token != TOKEN_WORD)
    {
        return token_string(token);
    }
    // Read the input again and store it in a buffer
    char *buffer = malloc(100 * sizeof(char));
    size_t buffer_pos = 0;
    char c = fgetc(lexer->input);
    if (c == '\'')
    {
        return handle_single_quote(lexer, buffer);
    }
    if (c == '\\')
    {
        c = fgetc(lexer->input);
    }
    while (is_continuous_word(c))
    {
        buffer[buffer_pos++] = c;
        c = fgetc(lexer->input);
        // If the buffer is too small, we increase it by 100
        if (buffer_pos % 100 == 0)
        {
            buffer = realloc(buffer, (buffer_pos + 100) * sizeof(char));
        }
    }
    buffer[buffer_pos] = '\0';
    fseek(lexer->input, lexer->pos, SEEK_SET);
    return buffer;
}

void lexer_savestate(struct lexer *lexer)
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
