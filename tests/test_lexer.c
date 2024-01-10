#include <criterion/criterion.h>

#include <../src/lexer/lexer.h>

Test(lexer, empty_string)
{
    struct lexer *lexer = create_lexer("");
    enum token token = lexer_peek(lexer);
    cr_assert(token == TOKEN_EOF);
}
