#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input/input.h"
#include "lexer/lexer.h"
#include "parser/parser.h"

int main(int argc, char **argv)
{
    int options = 0;
    FILE *input = get_input(argc, argv, &options);
    struct lexer *lexer = create_lexer(input);
    /*enum token token = TOKEN_IF;
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
    printf("\n");*/
    struct ast *ast = NULL;
    enum parser_status status = parse_input(&ast, lexer);
    if (status == PARSER_OK)
    {
        printf("Parser is OK!\n");
    }
    else
    {
        fprintf(stderr, "Parser is not ok :(\n");
        ast_free(ast);
        free_lexer(lexer);
        fclose(input);
        return 1;
    }
    if (check_prettyprint(&options))
    {
        print_ast(ast);
    }
    evaluate_ast(ast);
    ast_free(ast);
    free_lexer(lexer);
    fclose(input);
}
