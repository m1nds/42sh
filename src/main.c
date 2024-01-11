#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>

#include "input/input.h"
#include "lexer/lexer.h"

int main(int argc, char **argv)
{
    int options = 0;
    FILE *input = parse_input(argc, argv, &options);
    struct lexer *lexer = create_lexer(input);
    enum token token = TOKEN_IF;
    while (token != TOKEN_EOF)
    {
        printf("%s ", get_token_string(lexer));
        token = lexer_pop(lexer);
    }
    printf("\n");
    free_lexer(lexer);
}
