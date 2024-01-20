#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ast/variables.h"
#include "utils/hash_map.h"
#include "utils/itoa.h"
#include "utils/vector.h"

extern struct hash_map *hm;

void setup_args(int nb_args, char **args)
{
    struct vector *vec = vector_create(100);
    bool b;

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
        hash_map_insert(hm, strdup("@"), strdup(vec->data), &b);
        hash_map_insert(hm, strdup("*"), strdup(vec->data), &b);
    }

    free(vec);
}

void setup_args_n(int nb_args, char **args)
{
    bool b;
    for (int i = 0; i < nb_args; i++)
    {
        char *key = itoa_base(i + 1, "0123456789");
        hash_map_insert(hm, key, args[i], &b);
    }
}

void setup_variables(int nb_args, char **args)
{
    setup_args(nb_args, args);
    setup_args_n(nb_args, args);
    setup_args_nb(nb_args);
    setup_random();
    setup_uid();
    setup_pid();
    set_exit_value(0);
}
