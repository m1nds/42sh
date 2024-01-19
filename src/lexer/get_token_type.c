#define _POSIX_C_SOURCE 200809L
#include "get_token_type.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "handle_special_cases.h"

struct lexer_token_save match_word(char *word)
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
        return handle_escape(lexer);
    case '\n':
        lexer->prev = fgetc(lexer->input);
        out.curr_tok = TOKEN_RETURN;
        out.tok_str = strdup("\n");
        return out;
    case '|':
        out.curr_tok = TOKEN_REDIR_PIPE;
        new_c = fgetc(lexer->input);
        if (new_c == '|')
        {
            out.curr_tok = TOKEN_OR;
            out.tok_str = strdup("||");
            new_c = fgetc(lexer->input);
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
            out.curr_tok = TOKEN_AND;
            out.tok_str = strdup("&&");
            lexer->prev = fgetc(lexer->input);
        }
        else
        {
            out.curr_tok = TOKEN_NONE;
        }
        return out;
    case '!':
        out.curr_tok = TOKEN_NOT;
        out.tok_str = strdup("!");
        lexer->prev = fgetc(lexer->input);
        return out;
    default:
        break;
    }
    lexer->prev = c;
    return out;
}
