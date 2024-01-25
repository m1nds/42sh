#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exec/exec.h"
#include "input/input.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "variables/variables.h"

void free_heap(struct lexer *lexer, struct ast *ast, FILE *input)
{
    struct hash_map *hm_vars = get_variables();
    struct hash_map *hm_funcs = get_functions();
    ast_free(ast);
    free_lexer(lexer);
    fclose(input);
    hash_map_free(hm_vars);
    hash_map_free(hm_funcs);
}

int main_execution_loop(FILE *input, int options)
{
    struct lexer *lexer = create_lexer(input);
    /*enum token token = TOKEN_NONE;
    while (token != TOKEN_EOF)
    {
        token = lexer_peek(lexer).curr_tok;
        char *string = lexer_peek(lexer).tok_str;
        printf("Token : %i, String : %s\n", token, string);
        lexer_pop(lexer, true);
    }
    printf("\n");
    */
    struct ast *ast = NULL;
    enum parser_status status = parse_input(&ast, lexer);
    setup_variables(0, NULL);
    int return_code = 0;
    while (status == PARSER_OK)
    {
        if (check_prettyprint(&options))
        {
            print_ast(ast);
        }
        if (ast != NULL)
        {
            return_code = evaluate_ast(ast);
        }
        fflush(stdout);
        ast_free(ast);
        ast = NULL;
        enum token token = lexer_peek(lexer).curr_tok;
        if (token == TOKEN_EOF)
        {
            break;
        }
        status = parse_input(&ast, lexer);
        // printf("Parser is OK!\n");
    }
    if (status == PARSER_UNEXPECTED_TOKEN)
    {
        free_heap(lexer, ast, input);
        fprintf(stderr, "Parser is not ok :(\n");
        return 2;
    }
    free_heap(lexer, ast, input);
    return return_code;
}

int main(int argc, char **argv)
{
    int options = 0;
    FILE *input = get_input(argc, argv, &options);
    return main_execution_loop(input, options);
}
