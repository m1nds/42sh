#include "token.h"

char *token_string(enum token token)
{
    switch (token)
    {
    case TOKEN_IF:
        return "if";
    case TOKEN_THEN:
        return "then";
    case TOKEN_ELIF:
        return "elif";
    case TOKEN_ELSE:
        return "else";
    case TOKEN_FI:
        return "fi";
    case TOKEN_SEMICOLON:
        return ";";
    case TOKEN_SINGLE_QUOTE:
        return "'";
    case TOKEN_EOF:
        return "$";
    case TOKEN_RETURN:
        return "\\n";
    default:
        // Should only happen if it's a TOKEN_WORD
        return "use get_token_string instead";
    }
}
