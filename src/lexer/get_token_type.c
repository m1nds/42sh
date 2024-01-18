#define _POSIX_C_SOURCE 200809L
#include "get_token_type.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "handle_special_cases.h"

struct lexer_token_save match_word(char *word)
{
    struct lexer_token_save out;
    out.curr_tok = TOKEN_WORD;
    if (strcmp(word, "if") == 0)
    {
        out.curr_tok = TOKEN_IF;
        return out;
    }
    if (strcmp(word, "then") == 0)
    {
        out.curr_tok = TOKEN_THEN;
        return out;
    }
    if (strcmp(word, "elif") == 0)
    {
        out.curr_tok = TOKEN_ELIF;
        return out;
    }
    if (strcmp(word, "else") == 0)
    {
        out.curr_tok = TOKEN_ELSE;
        return out;
    }
    if (strcmp(word, "fi") == 0)
    {
        out.curr_tok = TOKEN_FI;
        return out;
    }
    if (strcmp(word, "for") == 0)
    {
        out.curr_tok = TOKEN_FOR;
        return out;
    }
    if (strcmp(word, "in") == 0)
    {
        out.curr_tok = TOKEN_IN;
        return out;
    }
    if (strcmp(word, "do") == 0)
    {
        out.curr_tok = TOKEN_DO;
        return out;
    }
    if (strcmp(word, "done") == 0)
    {
        out.curr_tok = TOKEN_DONE;
        return out;
    }
    return out;
}

struct lexer_token_save get_special_character(struct lexer *lexer, char c)
{
    struct lexer_token_save out;
    out.curr_tok = TOKEN_NONE;
    out.tok_str = NULL;
    switch (c)
    {
    case EOF:
    case '\0':
        out.curr_tok = TOKEN_EOF;
        return out;
    case ';':
        out.curr_tok = TOKEN_SEMICOLON;
        out.tok_str = strdup(";");
        lexer->prev = fgetc(lexer->input);
        return out;
    case '\'':
        return handle_single_quote(lexer, c);
    case '\\':
        return handle_escape(lexer, c);
    case '\n':
        lexer->prev = fgetc(lexer->input);
        out.curr_tok = TOKEN_RETURN;
        out.tok_str = strdup("\n");
        return out;
    case '|':
        out.curr_tok = TOKEN_REDIR_PIPE;
        out.tok_str = strdup("|");
        lexer->prev = fgetc(lexer->input);
        return out;
    default:
        break;
    }
    lexer->prev = c;
    return out;
}
