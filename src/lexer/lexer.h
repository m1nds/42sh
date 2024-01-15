#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "stack.h"
#include "token.h"

struct lexer
{
    FILE *input;
    size_t pos;
    struct stack *stack;
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
 * @def: Returns the next token without moving the lexer
 * @args lexer: the lexer to peek
 */
enum token lexer_peek(struct lexer *lexer);

/*
 * @def: Returns the next token and moves the lexer to be after that token
 * @args lexer: the lexer to pop
 */
enum token lexer_pop(struct lexer *lexer);

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
