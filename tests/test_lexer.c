#include <criterion/criterion.h>
#include <stdio.h>
#include <string.h>

#include "lexer/lexer.h"

Test(lexer, empty_string)
{
    struct lexer *lexer = create_lexer(fmemopen("", 1, "r+"));
    enum token token = lexer_peek(lexer);

    cr_expect_eq(token, TOKEN_EOF,
                 "Expected: %s. Got: %s. Token: %ld. Pos: %ld",
                 token_string(TOKEN_EOF), token_string(token), 0, lexer->pos);

    free_lexer(lexer);
}

Test(lexer, simple_if)
{
    struct lexer *lexer = create_lexer(fmemopen("if true ; then true ; else false ; fi", 1000, "r+"));

    enum token correct[] = { TOKEN_IF,   TOKEN_WORD, TOKEN_SEMICOLON,
                             TOKEN_THEN, TOKEN_WORD, TOKEN_SEMICOLON,
                             TOKEN_ELSE, TOKEN_WORD, TOKEN_SEMICOLON,
                             TOKEN_FI,   TOKEN_EOF };

    for (size_t i = 0; i < 11; i++)
    {
        enum token token = lexer_pop(lexer);
        cr_expect_eq(
            token, correct[i], "Expected: %s. Got: %s. Token: %ld. Pos: %ld",
            token_string(correct[i]), token_string(token), i, lexer->pos);
    }

    free_lexer(lexer);
}

Test(lexer, double_if)
{
    struct lexer *lexer = create_lexer(fmemopen("if if true; then true;"
                                       "else false; fi; then echo ACU; fi", 1000, "r+"));

    enum token correct[] = { TOKEN_IF,        TOKEN_IF,   TOKEN_WORD,
                             TOKEN_SEMICOLON, TOKEN_THEN, TOKEN_WORD,
                             TOKEN_SEMICOLON, TOKEN_ELSE, TOKEN_WORD,
                             TOKEN_SEMICOLON, TOKEN_FI,   TOKEN_SEMICOLON,
                             TOKEN_THEN,      TOKEN_WORD, TOKEN_WORD,
                             TOKEN_SEMICOLON, TOKEN_FI,   TOKEN_EOF };

    for (size_t i = 0; i < 18; i++)
    {
        enum token token = lexer_pop(lexer);
        cr_expect_eq(
            token, correct[i], "Expected: %s. Got: %s. Token: %ld. Pos: %ld",
            token_string(correct[i]), token_string(token), i, lexer->pos);
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
        enum token token = lexer_pop(lexer);
        cr_expect_eq(
            token, correct[i], "Expected: %s. Got: %s. Token: %ld. Pos: %ld",
            token_string(correct[i]), token_string(token), i, lexer->pos);
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
        enum token token = lexer_pop(lexer);
        cr_expect_eq(
            token, correct[i], "Expected: %s. Got: %s. Token: %ld. Pos: %ld",
            token_string(correct[i]), token_string(token), i, lexer->pos);
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
        enum token token = lexer_pop(lexer);
        cr_expect_eq(
            token, correct[i], "Expected: %s. Got: %s. Token: %ld. Pos: %ld",
            token_string(correct[i]), token_string(token), i, lexer->pos);
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
        enum token token = lexer_pop(lexer);
        cr_expect_eq(
            token, correct[i], "Expected: %s. Got: %s. Token: %ld. Pos: %ld",
            token_string(correct[i]), token_string(token), i, lexer->pos);
    }

    free_lexer(lexer);
}
