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
    TOKEN_FOR,
    TOKEN_IN,
    TOKEN_DO,
    TOKEN_DONE,
    TOKEN_WORD,
    TOKEN_EOF = 100,
    TOKEN_SEMICOLON,
    TOKEN_RETURN,
    TOKEN_STDIN,
    TOKEN_REDIR_PIPE,
    //......
};

#endif /* TOKEN_H */
