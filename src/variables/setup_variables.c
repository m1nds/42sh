#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils/hash_map.h"
#include "utils/itoa.h"
#include "utils/vector.h"
#include "variables/variables.h"

void setup_args(int nb_args, char **args)
{
    if (args == NULL)
    {
        return;
    }
    struct vector *vec = vector_create(100);
    if (*args)
    {
        vector_append_string(vec, args[0]);
    }

    for (int i = 1; i < nb_args; i++)
    {
        vector_append(vec, ' ');
        vector_append_string(vec, args[i]);
    }

    vector_append(vec, '\0');

    if (strcmp(vec->data, "") != 0)
    {
        hash_map_insert(hm_vars, strdup("@"), strdup(vec->data));
        hash_map_insert(hm_vars, strdup("*"), strdup(vec->data));
    }

    vector_destroy(vec);
}

void setup_args_n(int nb_args, char **args)
{
    if (args == NULL)
    {
        return;
    }

    for (int i = 0; i < nb_args; i++)
    {
        char *key = itoa_base(i + 1);
        hash_map_insert(hm_vars, key, args[i]);
    }
}

void setup_value(char *key, char *value)
{
    hash_map_insert(hm_vars, key, value);
}

void set_exit_value(int value)
{
    char *str = itoa_base(value);
    hash_map_insert(hm_vars, strdup("?"), str);
}

struct hash_map *get_variables(void)
{
    return hm_vars;
}

struct hash_map *get_functions(void)
{
    return hm_funcs;
}

void setup_variables(int nb_args, char **args)
{
    srand(time(NULL));
    hm_vars = hash_map_init(25);
    hm_funcs = hash_map_init(25);
    setup_args(nb_args, args);
    setup_args_n(nb_args, args);
    setup_nb_args(nb_args);
    setup_random(NULL);
    setup_uid();
    setup_pid();
    set_exit_value(0);
}
