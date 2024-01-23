#define _POSIX_C_SOURCE 200809L
#include "lexer/get_token_type.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "lexer/handle_special_cases.h"

struct lexer_token_save match_word(struct lexer *lexer, char *word)
{
    char *words[] = { "if",    "then", "elif", "else", "fi",  "while",
                      "until", "for",  "in",   "do",   "done" };
    enum token tokens[] = { TOKEN_IF, TOKEN_THEN,  TOKEN_ELIF,  TOKEN_ELSE,
                            TOKEN_FI, TOKEN_WHILE, TOKEN_UNTIL, TOKEN_FOR,
                            TOKEN_IN, TOKEN_DO,    TOKEN_DONE };
    size_t len = 11;
    struct lexer_token_save out;
    out.curr_tok = TOKEN_WORD;
    for (size_t i = 0; i < len; i++)
    {
        if (strcmp(word, words[i]) == 0)
        {
            out.curr_tok = tokens[i];
            return out;
        }
    }
    return handle_io_number(lexer, word);
}

struct lexer_token_save fill_out(struct lexer *lexer,
                                 struct lexer_token_save out, enum token token)
{
    switch (token)
    {
    case TOKEN_EOF:
        out.curr_tok = token;
        return out;
    case TOKEN_SEMICOLON:
        out.curr_tok = token;
        out.tok_str = strdup(";");
        lexer->prev = -10;
        return out;
    case TOKEN_RETURN:
        out.curr_tok = token;
        out.tok_str = strdup("\n");
        lexer->prev = -10;
        return out;
    case TOKEN_NOT:
        out.curr_tok = token;
        out.tok_str = strdup("!");
        lexer->prev = -10;
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
    switch (c)
    {
    case EOF:
    case '\0':
        return fill_out(lexer, out, TOKEN_EOF);
    case ';':
        return fill_out(lexer, out, TOKEN_SEMICOLON);
    case '<':
    case '>':
        return handle_redirects(lexer, c);
    case '\\':
        return handle_escape(lexer);
    case '\n':
        return fill_out(lexer, out, TOKEN_RETURN);
    case '|':
        return handle_pipe_or(lexer);
    case '&':
        return handle_ands(lexer);
    case '!':
        return fill_out(lexer, out, TOKEN_NOT);
    default:
        break;
    }
    lexer->prev = c;
    return out;
}
