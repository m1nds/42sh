#include <criterion/criterion.h>
#include <stdio.h>
#include <string.h>

#include "lexer/lexer.h"

Test(lexer, empty_string)
{
    struct lexer *lexer = create_lexer(fmemopen("", 1, "r+"));
    struct lexer_token_save t = lexer_peek(lexer);
    enum token token = t.curr_tok;
    cr_expect_eq(token, TOKEN_EOF, "Got: %s. Token: %ld. Pos: %ld", t.tok_str,
                 0, lexer->pos);

    free_lexer(lexer);
}

Test(lexer, simple_if)
{
    struct lexer *lexer = create_lexer(
        fmemopen("if true ; then true ; else false ; fi", 1000, "r+"));

    enum token correct[] = { TOKEN_IF,   TOKEN_WORD, TOKEN_SEMICOLON,
                             TOKEN_THEN, TOKEN_WORD, TOKEN_SEMICOLON,
                             TOKEN_ELSE, TOKEN_WORD, TOKEN_SEMICOLON,
                             TOKEN_FI,   TOKEN_EOF };

    for (size_t i = 0; i < 11; i++)
    {
        struct lexer_token_save t = lexer_peek(lexer);
        enum token token = t.curr_tok;
        lexer_pop(lexer, true);
        cr_expect_eq(token, correct[i], "Got: %s. Token: %ld. Pos: %ld",
                     t.tok_str, i, lexer->pos);
    }

    free_lexer(lexer);
}

Test(lexer, double_if)
{
    struct lexer *lexer =
        create_lexer(fmemopen("if if true; then true;"
                              "else false; fi; then echo ACU; fi",
                              1000, "r+"));

    enum token correct[] = { TOKEN_IF,        TOKEN_IF,   TOKEN_WORD,
                             TOKEN_SEMICOLON, TOKEN_THEN, TOKEN_WORD,
                             TOKEN_SEMICOLON, TOKEN_ELSE, TOKEN_WORD,
                             TOKEN_SEMICOLON, TOKEN_FI,   TOKEN_SEMICOLON,
                             TOKEN_THEN,      TOKEN_WORD, TOKEN_WORD,
                             TOKEN_SEMICOLON, TOKEN_FI,   TOKEN_EOF };

    for (size_t i = 0; i < 18; i++)
    {
        struct lexer_token_save t = lexer_peek(lexer);
        enum token token = t.curr_tok;
        lexer_pop(lexer, true);
        cr_expect_eq(token, correct[i], "Got: %s. Token: %ld. Pos: %ld",
                     t.tok_str, i, lexer->pos);
    }

    free_lexer(lexer);
}

Test(lexer, single_quotes)
{
    char *string = "if 'true; then true; else false; fi;' then echo ACU; fi";
    struct lexer *lexer = create_lexer(fmemopen(string, strlen(string), "r+"));

    enum token correct[] = { TOKEN_IF,   TOKEN_WORD, TOKEN_THEN,
                             TOKEN_WORD, TOKEN_WORD, TOKEN_SEMICOLON,
                             TOKEN_FI,   TOKEN_EOF };

    for (size_t i = 0; i < 8; i++)
    {
        struct lexer_token_save t = lexer_peek(lexer);
        enum token token = t.curr_tok;
        lexer_pop(lexer, true);
        cr_expect_eq(token, correct[i], "Got: %s. Token: %ld. Pos: %ld",
                     t.tok_str, i, lexer->pos);
    }

    free_lexer(lexer);
}

Test(lexer, missing_single_quote)
{
    char *string = "if true; then true;else 'false; fi; then echo ACU; fi";
    struct lexer *lexer = create_lexer(fmemopen(string, strlen(string), "r+"));

    enum token correct[] = { TOKEN_IF,   TOKEN_WORD, TOKEN_SEMICOLON,
                             TOKEN_THEN, TOKEN_WORD, TOKEN_SEMICOLON,
                             TOKEN_ELSE, TOKEN_STDIN };

    for (size_t i = 0; i < 8; i++)
    {
        struct lexer_token_save t = lexer_peek(lexer);
        enum token token = t.curr_tok;
        lexer_pop(lexer, true);
        cr_expect_eq(token, correct[i], "Got: %s. Token: %ld. Pos: %ld",
                     t.tok_str, i, lexer->pos);
    }

    free_lexer(lexer);
}

Test(lexer, comment)
{
    char *string = "if true; then tr#ue; #test\n"
                   "else false; fi; #then echo ACU; fi";
    struct lexer *lexer = create_lexer(fmemopen(string, strlen(string), "r+"));

    enum token correct[] = { TOKEN_IF,   TOKEN_WORD,      TOKEN_SEMICOLON,
                             TOKEN_THEN, TOKEN_WORD,      TOKEN_SEMICOLON,
                             TOKEN_ELSE, TOKEN_WORD,      TOKEN_SEMICOLON,
                             TOKEN_FI,   TOKEN_SEMICOLON, TOKEN_EOF };

    for (size_t i = 0; i < 12; i++)
    {
        struct lexer_token_save t = lexer_peek(lexer);
        enum token token = t.curr_tok;
        lexer_pop(lexer, true);
        cr_expect_eq(token, correct[i], "Got: %s. Token: %ld. Pos: %ld",
                     t.tok_str, i, lexer->pos);
    }

    free_lexer(lexer);
}

Test(lexer, returns)
{
    char *string = "if true; then tr#ue;\nelse \n\n\nfalse; fi;";
    struct lexer *lexer = create_lexer(fmemopen(string, strlen(string), "r+"));

    enum token correct[] = { TOKEN_IF,        TOKEN_WORD,   TOKEN_SEMICOLON,
                             TOKEN_THEN,      TOKEN_WORD,   TOKEN_SEMICOLON,
                             TOKEN_RETURN,    TOKEN_ELSE,   TOKEN_RETURN,
                             TOKEN_RETURN,    TOKEN_RETURN, TOKEN_WORD,
                             TOKEN_SEMICOLON, TOKEN_FI,     TOKEN_SEMICOLON,
                             TOKEN_EOF };

    for (size_t i = 0; i < 16; i++)
    {
        struct lexer_token_save t = lexer_peek(lexer);
        enum token token = t.curr_tok;
        lexer_pop(lexer, true);
        cr_expect_eq(token, correct[i], "Got: %s. Token: %ld. Pos: %ld",
                     t.tok_str, i, lexer->pos);
    }

    free_lexer(lexer);
}
