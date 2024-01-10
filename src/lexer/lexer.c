#include "lexer.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct lexer *create_lexer(char *input)
{
    struct lexer *result = calloc(1, sizeof(struct lexer));
    //Check for NULL
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

enum token match_word(char *word)
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
        return TOKEN_IF;
    }
    if (strcmp(word, "\n") == 0)
    {
        return TOKEN_RETURN;
    }
    if (strcmp(word, ";") == 0)
    {
        return TOKEN_SEMICOLON;
    }
    if (strcmp(word, "'") == 0)
    {
        return TOKEN_SINGLE_QUOTE;
    }
    return TOKEN_WORD;
}

/*
 * Returns true if the current character belongs in the word
 * False if not
 */
bool is_continuous_word(char *input, size_t pos)
{
    return input[pos] != ' '
        && input[pos] != ';'
        && input[pos] != '\'';
}

enum token get_next_token(struct lexer *lexer)
{
    char *word = calloc(100, sizeof(char));
    size_t word_pos = 0;
    // Remove all spaces before the word
    while (lexer->input[lexer->pos] == ' ')
    {
        lexer->pos++;
    }
    // Not sure if \0 should count as EOF but it should be fine
    if (lexer->input[lexer->pos] == EOF || lexer->input[lexer->pos] == '\0')
    {
        return TOKEN_EOF;
    }
    size_t new_pos = lexer->pos;
    // Very basic version, won't work later
    while (is_continuous_word(lexer->input, new_pos))
    {
        word[word_pos] = lexer->input[new_pos];
        new_pos++;
        word_pos++;
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

enum token lexer_pop(struct lexer *lexer)
{
    enum token token = get_next_token(lexer);
    //Update position of lexer
    while (is_continuous_word(lexer->input, lexer->pos))
    {
        lexer->pos++;
    }
    return token;
}

char *get_token_string(struct lexer *lexer)
{
    enum token token = lexer_peek(lexer);
    if (token != TOKEN_WORD)
    {
        return token_string(token);
    }
    // Read the input again and store it in a buffer
    return "TODO";
}
