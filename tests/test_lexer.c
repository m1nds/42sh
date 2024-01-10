#include <criterion/criterion.h>

#include "../src/lexer/lexer.h"

Test(lexer, empty_string)
{
    struct lexer *lexer = create_lexer("");
    enum token token = lexer_peek(lexer);

    cr_expect_eq(token, TOKEN_EOF,
                 "Expected: %s. Got: %s. Token: %ld. Pos: %ld",
                 token_string(TOKEN_EOF), token_string(token), 0, lexer->pos);

    free_lexer(lexer);
}

Test(lexer, simple_if)
{
    struct lexer *lexer = create_lexer("if true ; then true ; else false ; fi");
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
