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

struct lexer_token_save fill_out(struct lexer *lexer,
                                 struct lexer_token_save out, enum token token,
                                 char *c)
{
    switch (token)
    {
    case TOKEN_EOF:
        out.curr_tok = token;
        return out;
    case TOKEN_SEMICOLON:
        out.curr_tok = token;
        out.tok_str = strdup(";");
        lexer->prev = fgetc(lexer->input);
        return out;
    case TOKEN_RETURN:
        out.curr_tok = token;
        out.tok_str = strdup("\n");
        lexer->prev = fgetc(lexer->input);
        return out;
    case TOKEN_REDIR_PIPE:
        out.curr_tok = token;
        *c = fgetc(lexer->input);
        return out;
    case TOKEN_OR:
        out.curr_tok = token;
        out.tok_str = strdup("||");
        *c = fgetc(lexer->input);
        return out;
    case TOKEN_AND:
        out.curr_tok = token;
        out.tok_str = strdup("&&");
        lexer->prev = fgetc(lexer->input);
        return out;
    case TOKEN_NOT:
        out.curr_tok = token;
        out.tok_str = strdup("!");
        lexer->prev = fgetc(lexer->input);
        return out;
    default:
        break;
    }
    return out;
}

struct lexer_token_save get_special_character(struct lexer *lexer, char c)
{
    struct lexer_token_save out;
    out.curr_tok = TOKEN_NONE;
    out.tok_str = NULL;
    char new_c;
    switch (c)
    {
    case EOF:
    case '\0':
        return fill_out(lexer, out, TOKEN_EOF, &c);
    case ';':
        return fill_out(lexer, out, TOKEN_SEMICOLON, &c);
    case '\'':
        return handle_single_quote(lexer, c);
    case '\\':
        return handle_escape(lexer, c);
    case '\n':
        return fill_out(lexer, out, TOKEN_RETURN, &c);
    case '|':
        out = fill_out(lexer, out, TOKEN_REDIR_PIPE, &new_c);
        if (new_c == '|')
        {
            out = fill_out(lexer, out, TOKEN_OR, &new_c);
        }
        else
        {
            out.tok_str = strdup("|");
        }
        lexer->prev = new_c;
        return out;
    case '&':
        new_c = fgetc(lexer->input);
        if (new_c == '&')
        {
            out = fill_out(lexer, out, TOKEN_AND, &c);
        }
        else
        {
            out.curr_tok = TOKEN_NONE;
        }
        return out;
    case '!':
        return fill_out(lexer, out, TOKEN_NOT, &c);
    default:
        break;
    }
    lexer->prev = c;
    return out;
}
