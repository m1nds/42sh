#ifndef TOKEN_H
#define TOKEN_H

enum token
{
    TOKEN_IF = 0,
    TOKEN_FI,
    TOKEN_ELSE,
    TOKEN_SEMICOLON,
    TOKEN_EOF,
    TOKEN_WORD,
    TOKEN_THEN,
    TOKEN_ELIF,
    TOKEN_RETURN,
    TOKEN_STDIN,
    TOKEN_NOT,
    TOKEN_WHILE,
    TOKEN_UNTIL,
    TOKEN_FOR,
    TOKEN_DO,
    TOKEN_DONE,
    TOKEN_REDIR_INPUT = 100, // To facilitate regrouping all redirects
    TOKEN_REDIR_OUTPUT,
    TOKEN_REDIR_APPEND,
    TOKEN_REDIR_INFD,
    TOKEN_REDIR_OUTFD,
    TOKEN_REDIR_BOTHFD,
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
