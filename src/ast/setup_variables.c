#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ast/variables.h"
#include "utils/hash_map.h"
#include "utils/itoa.h"
#include "utils/vector.h"

struct hash_map *hm;

void setup_args(int nb_args, char **args)
{
    if (args == NULL)
    {
        return;
    }
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

    vector_destroy(vec);
}

void setup_args_n(int nb_args, char **args)
{
    if (args == NULL)
    {
        return;
    }
    bool b;
    for (int i = 0; i < nb_args; i++)
    {
        char *key = itoa_base(i + 1);
        hash_map_insert(hm, key, args[i], &b);
    }
}

void setup_value(char *key, char *value)
{
    bool b;
    hash_map_insert(hm, key, value, &b);
}

void set_exit_value(int value)
{
    bool b;
    hash_map_insert(hm, strdup("?"), itoa_base(value), &b);
}

void setup_variables(int nb_args, char **args)
{
    hm = hash_map_init(25);
    setup_args(nb_args, args);
    setup_args_n(nb_args, args);
    setup_nb_args(nb_args);
    setup_random();
    setup_uid();
    setup_pid();
    set_exit_value(0);
}
