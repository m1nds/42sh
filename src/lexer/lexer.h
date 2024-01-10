#ifndef PARSER_H
#define PARSER_H

struct lexer
{
    char *input;
    size_t pos;
}

/*
 * @def: creates the lexer and returns it
 * @args input: input that needs to be parsed
 */
struct lexer *create_lexer(char *input);

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

#endif /* PARSER_H */
