#ifndef TOKEN_H
#define TOKEN_H

enum token
{
    TOKEN_NONE = -1,
    TOKEN_IF,
    TOKEN_FI,
    TOKEN_ELSE,
    TOKEN_THEN,
    TOKEN_ELIF,
    TOKEN_WORD,
    TOKEN_EOF = 10,
    TOKEN_SEMICOLON,
    TOKEN_RETURN,
    TOKEN_STDIN,
    TOKEN_REDIR_PIPE,
    //......
};

/*
 * @def: Returns a string based on the token
 * Should not be used if token is TOKEN_WORD
 * @args token: The token to be stringified
 */
char *token_string(enum token token);

#endif /* TOKEN_H */
