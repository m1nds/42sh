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

static int get_arg(int argc, char **argv, char ***r)
{
    int i = 1;
    while (i < argc)
    {
        if (strcmp(argv[i], "-c") == 0)
        {
            i += 2;
            continue;
        }
        if (strcmp(argv[i], "--pretty-print") != 0)
        {
            break;
        }
        i++;
    }
    if (i < argc)
    {
        *r = argv + i;
        i = argc - i;
    }
    else
    {
        *r = NULL;
        i = 0;
    }
    return i;
}

int main(int argc, char **argv)
{
    int options = 0;
    FILE *input = get_input(argc, argv, &options);
    char **r = NULL;
    int nb = get_arg(argc, argv, &r);
    setup_variables(nb, r);
    int out = main_execution_loop(input, options);
    struct hash_map *hm_vars = get_variables();
    struct hash_map *hm_funcs = get_functions();
    hash_map_free(hm_vars);
    hash_map_free(hm_funcs);
    return out;
}
