#ifndef TOKEN_H
#define TOKEN_H

enum token
{
    TOKEN_IF,
    TOKEN_FI,
    TOKEN_ELSE,
    TOKEN_SEMICOLON,
    TOKEN_EOF,
    TOKEN_WORD,
    TOKEN_THEN,
    TOKEN_ELIF,
    TOKEN_SINGLE_QUOTE,
    TOKEN_RETURN
    //......
}

/*
 * @def: Returns a string based on the token
 * Should not be used if token is TOKEN_WORD
 * @args token: The token to be stringified
 */
char *token_string(enum token token);

#endif /* TOKEN_H */
