#ifndef VARIABLES_H
#define VARIABLES_H

#define _POSIX_C_SOURCE 200809L
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils/hash_map.h"
#include "utils/itoa.h"

extern struct hash_map *hm_vars;

static inline char *get_exit_value()
{
    return hash_map_get(hm_vars, "?");
}

static inline void setup_nb_args(int nb_args)
{
    char *value = itoa_base(nb_args);
    hash_map_insert(hm_vars, strdup("#"), value);
}

static inline char *get_nb_args()
{
    return hash_map_get(hm_vars, "#");
}

static inline void setup_pid()
{
    char *value = itoa_base(getpid());
    hash_map_insert(hm_vars, strdup("$"), value);
}

static inline char *get_pid()
{
    return hash_map_get(hm_vars, "$");
}

static inline void setup_random()
{
    char *value = itoa_base((int)((rand() * (size_t)hm_vars) % 32768));
    hash_map_insert(hm_vars, strdup("RANDOM"), value);
}

static inline char *get_random()
{
    char *out = hash_map_get(hm_vars, "RANDOM");
    setup_random();
    return out;
}

static inline void setup_uid()
{
    char *out = itoa_base(getuid());
    hash_map_insert(hm_vars, strdup("UID"), out);
}

static inline char *get_uid()
{
    return hash_map_get(hm_vars, "UID");
}

void setup_value(char *key, char *value);

void set_exit_value(int value);

void setup_variables(int nb_args, char **args);

#endif /* ! VARIABLES_H*/
