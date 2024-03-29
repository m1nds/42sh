#ifndef LEXER_H
#define LEXER_H

#define CHAR_NONE -10
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "lexer/token.h"
#include "utils/vector.h"

struct lexer_token_save
{
    enum token curr_tok;
    char *tok_str;
};

struct lexer
{
    FILE *input;
    size_t pos;
    char prev;
    struct lexer_token_save ls;
    struct lexer_token_save ls_next;
};

/*
 * @def: creates the lexer and returns it
 * @args input: input that needs to be parsed
 */
struct lexer *create_lexer(FILE *input);

/*
 * @def: frees the lexer
 * @args lexer: the lexer that needs to be freed
 */
void free_lexer(struct lexer *lexer);

/*
 * @def: The main loop of the lexer. Stores the word in vec.
 */
struct lexer_token_save main_loop(struct lexer *lexer, struct vector *vec);

/*
 * @def: Returns the next token without moving the lexer
 * @args lexer: the lexer to peek
 */
struct lexer_token_save lexer_peek(struct lexer *lexer);

/*
 * @def: Returns the token after the next token without moving the lexer
 * @args lexer: the lexer to next peek
 */
struct lexer_token_save lexer_next_peek(struct lexer *lexer);

/*
 * @def: Returns the next token and moves the lexer to be after that token
 * @args lexer: the lexer to pop
 */
void lexer_pop(struct lexer *lexer, bool to_free);

/*
 * @def: Returns a string version of the next token
 * @args lexer: the lexer that holds the current token
 */
char *get_token_string(struct lexer *lexer);

/*
 * @def: Save the current file position in the stack
 * @args lexer: the lexer to save the current file position
 */
void lexer_savestate(struct lexer *lexer);

/*
 * @def: Loads the first file position of the stack in the lexer
 * @args lexer: the lexer to load the file position
 */
bool lexer_loadstate(struct lexer *lexer);

#endif /* LEXER_H */
