#include "lexer.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct lexer *create_lexer(char *input)
{
    struct lexer *result = calloc(1, sizeof(struct lexer));
    // Check for NULL
    if (result == NULL)
    {
        return NULL;
    }
    result->input = input;
    result->pos = 0;
    return result;
}

void free_lexer(struct lexer *lexer)
{
    if (lexer != NULL)
    {
        free(lexer);
    }
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
    while (lexer->input[lexer->pos] != '\n' && lexer->input[lexer->pos] != EOF
           && lexer->input[lexer->pos] != '\0')
    {
        lexer->pos++;
    }
    if (lexer->input[lexer->pos] == '\n')
    {
        lexer->pos++;
    }
}

static enum token check_single_quote(struct lexer *lexer)
{
    size_t pos = lexer->pos + 1;
    while (lexer->input[pos] != '\'')
    {
        if (lexer->input[pos] == EOF || lexer->input[pos] == '\0')
        {
            return TOKEN_STDIN;
        }
        pos++;
    }
    return TOKEN_WORD;
}

static enum token get_next_token(struct lexer *lexer)
{
    char *word = calloc(100, sizeof(char));
    size_t word_pos = 0;
    // Remove all spaces before the word
    while (lexer->input[lexer->pos] == ' ')
    {
        lexer->pos++;
    }
    if (lexer->input[lexer->pos] == '#')
    {
        ignore_line(lexer);
    }
    // Not sure if \0 should count as EOF but it should be fine
    if (lexer->input[lexer->pos] == EOF || lexer->input[lexer->pos] == '\0')
    {
        return TOKEN_EOF;
    }
    if (lexer->input[lexer->pos] == ';')
    {
        return TOKEN_SEMICOLON;
    }
    if (lexer->input[lexer->pos] == '\n')
    {
        return TOKEN_RETURN;
    }
    if (lexer->input[lexer->pos] == '\'')
    {
        // Check if the single quote is closed or not
        return check_single_quote(lexer);
    }
    // Special case if there is a \n after \, need to check later
    if (lexer->input[lexer->pos] == '\\')
    {
        return TOKEN_WORD;
    }
    size_t pos = lexer->pos;
    // Very basic version, won't work later
    while (is_continuous_word(lexer->input[pos]))
    {
        word[word_pos++] = lexer->input[pos++];
        // Failsafe if a word is too big
        if (word_pos >= 100)
        {
            return TOKEN_WORD;
        }
    }
    enum token result = match_word(word);
    free(word);
    return result;
}

enum token lexer_peek(struct lexer *lexer)
{
    return get_next_token(lexer);
}

static void skip_single_quote(struct lexer *lexer)
{
    lexer->pos++;
    while (lexer->input[lexer->pos] != '\'')
    {
        lexer->pos++;
    }
    lexer->pos++;
}

enum token lexer_pop(struct lexer *lexer)
{
    enum token token = get_next_token(lexer);
    // Update position of lexer
    if (token == TOKEN_WORD && lexer->input[lexer->pos] == '\'')
    {
        skip_single_quote(lexer);
        return token;
    }
    while (is_continuous_word(lexer->input[lexer->pos]))
    {
        lexer->pos++;
    }
    if (token == TOKEN_RETURN || token == TOKEN_SEMICOLON)
    {
        lexer->pos++;
    }
    return token;
}

char *handle_single_quote(struct lexer *lexer, char *buffer)
{
    size_t buffer_pos = 0;
    size_t pos = lexer->pos + 1;
    while (lexer->input[pos] != '\'')
    {
        buffer[buffer_pos++] = lexer->input[pos++];
        if (lexer->input[pos] == EOF || lexer->input[pos] == '\0')
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
    size_t pos = lexer->pos;
    size_t buffer_pos = 0;
    if (lexer->input[lexer->pos] == '\'')
    {
        return handle_single_quote(lexer, buffer);
    }
    if (lexer->input[pos] == '\\')
    {
        pos++;
    }
    while (is_continuous_word(lexer->input[pos]))
    {
        buffer[buffer_pos++] = lexer->input[pos++];
        // If the buffer is too small, we increase it by 100
        if (buffer_pos % 100 == 0)
        {
            buffer = realloc(buffer, (buffer_pos + 100) * sizeof(char));
        }
    }
    buffer[buffer_pos] = '\0';
    return buffer;
}
