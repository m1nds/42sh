#include "lexer.h"

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
    result->pos = ftell(input);
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

char lexer_step(struct lexer *lexer, int step)
{
    lexer->pos += step;
    return fgetc(lexer->input);
}

static enum token match_word(char *word)
{
    if (strcmp(word, "if") == 0)
    {
        return TOKEN_IF;
    }
    if (strcmp(word, "then") == 0)
    {
        return TOKEN_THEN;
    }
    if (strcmp(word, "elif") == 0)
    {
        return TOKEN_ELIF;
    }
    if (strcmp(word, "else") == 0)
    {
        return TOKEN_ELSE;
    }
    if (strcmp(word, "fi") == 0)
    {
        return TOKEN_FI;
    }
    return TOKEN_WORD;
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
    char c = lexer_step(lexer, 1);
    while (c != '\n' && c != EOF && c != '\0')
    {
        c = lexer_step(lexer, 1);
    }

    if (c == '\n')
    {
        lexer->pos++;
    }
    /*else
    {
        ungetc(c, lexer->input);
    }*/
}

static enum token check_single_quote(struct lexer *lexer)
{
    char c = fgetc(lexer->input);
    while (c != '\'')
    {
        if (c == EOF || c == '\0')
        {
            fseek(lexer->input, lexer->pos, SEEK_SET);
            return TOKEN_STDIN;
        }
        c = fgetc(lexer->input);
    }
    fseek(lexer->input, lexer->pos, SEEK_SET);
    return TOKEN_WORD;
}

char skip_blanks(struct lexer *lexer)
{
    char c = lexer_step(lexer, 0);

    while (isblank(c))
    {
        c = lexer_step(lexer, 1);
    }

    return c;
}

static enum token get_next_token(struct lexer *lexer)
{
    // Remove all spaces before the word
    char c = skip_blanks(lexer);
    
    // Skip comment line
    /*if (c == '#')
    {
        ignore_line(lexer);
        c = lexer_step(lexer, 0);
    }
    // Not sure if \0 should count as EOF but it should be fine
    if (c == EOF || c == '\0')
    {
        return TOKEN_EOF;
    }
    if (c == ';')
    {
        return TOKEN_SEMICOLON;
    }
    if (c == '\n')
    {
        return TOKEN_RETURN;
    }
    if (c == '\'')
    {
        // Check if the single quote is closed or not
        return check_single_quote(lexer);
    }
    // Special case if there is a \n after \, need to check later
    if (c == '\\')
    {
        return TOKEN_WORD;
    }*/

    switch (c)
    {
    case '#':
        ignore_line(lexer);
        c = lexer_step(lexer, 0);
        break;
    case EOF:
    case '\0':
        return TOKEN_EOF;
    case ';':
        return TOKEN_SEMICOLON;
    case '\'':
        return check_single_quote(lexer);
    case '\\':
        return TOKEN_WORD;
    }

    struct vector *vec = vector_create(100);
    while (is_continuous_word(c))
    {
        vector_append(vec, c);
        c = lexer_step(lexer, 0);
    }

    enum token result = match_word(vec->data);
    vector_destroy(vec);
    return result;

    /*char *word = calloc(100, sizeof(char));
    size_t word_pos = 0;
    // Very basic version, won't work later
    while (is_continuous_word(c))
    {
        word[word_pos++] = c;
        c = fgetc(lexer->input);
        // Failsafe if a word is too big
        if (word_pos >= 100)
        {
            return TOKEN_WORD;
        }
    }

    enum token result = match_word(word);
    free(word);
    return result;*/
}

enum token lexer_peek(struct lexer *lexer)
{
    if (lexer->ls.curr_tok == TOKEN_NONE)
    {
        lexer->ls.curr_tok = get_next_token(lexer);
    }

    return lexer->ls.curr_tok;
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

enum token lexer_pop(struct lexer *lexer)
{
    enum token out;

    if (lexer->ls.curr_tok == TOKEN_NONE)
    {
        out = get_next_token(lexer);

        lexer->ls.curr_tok = get_next_token(lexer);
        return out;
    }

    out = lexer->ls.curr_tok;
    lexer->ls.curr_tok = get_next_token(lexer);

    return out;
}

static char *handle_single_quote(struct lexer *lexer, char *buffer)
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
