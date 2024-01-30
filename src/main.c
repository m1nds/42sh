#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exec/exec.h"
#include "input/input.h"
#include "lexer/lexer.h"
#include "main_loop/main_loop.h"
#include "parser/parser.h"
#include "variables/variables.h"

int main(int argc, char **argv)
{
    int options = 0;
    FILE *input = get_input(argc, argv, &options);
    setup_variables(0, NULL);
    int out = main_execution_loop(input, options);
    struct hash_map *hm_vars = get_variables();
    struct hash_map *hm_funcs = get_functions();
    hash_map_free(hm_vars);
    hash_map_free(hm_funcs);
    return out;
}
