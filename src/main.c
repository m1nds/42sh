#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input/input.h"
#include "lexer/lexer.h"

int main(int argc, char **argv)
{
    int options = 0;
    FILE *input = get_input(argc, argv, &options);
    struct lexer *lexer = create_lexer(input);
    enum token token = TOKEN_IF;
    while (token != TOKEN_EOF)
    {
        char *string = get_token_string(lexer);
        printf("%s ", string);
        token = lexer_pop(lexer);
        if (token == TOKEN_WORD)
        {
            free(string);
        }
    }
    printf("\n");
    free_lexer(lexer);
    fclose(input);
}
