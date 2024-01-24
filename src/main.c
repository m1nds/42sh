#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast/variables.h"
#include "exec/exec.h"
#include "input/input.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "builtin/builtin.h"

extern struct hash_map *hm;

void free_heap(struct lexer *lexer, struct ast *ast, FILE *input)
{
    ast_free(ast);
    free_lexer(lexer);
    fclose(input);
    hash_map_free(hm);
}

int main()
{
    char *args1[] = {"/home/../home/zobes", NULL};
    char *args2[] = {"/home/../../zobes", NULL};

    //char *args4[] = {"/home///////././//./zobes", NULL};
    //char *args5[] = {"/home///////././//./zobes", NULL};

    cd_builtin(args1);
    cd_builtin(args2);
}
