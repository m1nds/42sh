#include <criterion/criterion.h>

#include <../src/lexer/lexer.h>

Test(ast, empty_string)
{
    struct lexer *lexer = create_lexer("");
    enum token token = lexer_peek(lexer);
    cr_assert(token == TOKEN_EOF);
}
