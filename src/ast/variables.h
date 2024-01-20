#ifndef VARIABLES_H
#define VARIABLES_H

#define _POSIX_C_SOURCE 200809L
#include <stdbool.h>
#include <stdlib.h>

#include "utils/hash_map.h"
#include "utils/itoa.h"

extern struct hash_map *hm;

static inline void set_exit_value(int value)
{
    bool b;
    hash_map_insert(hm, strdup("?"), itoa_base(value, "0123456789"), &b);
}

static inline char *get_exit_value()
{
    return hash_map_get(hm, "?");
}

static inline void setup_nb_args(int nb_args)
{
    bool b;
    hash_map_insert(hm, strdup("#"), itoa_base(nb_args, "0123456789"), &b);
}

static inline char *get_nb_args()
{
    return hash_map_get(hm, "#");
}

static inline void setup_pid()
{
    bool b;
    hash_map_insert(hm, strdup("$"), itoa_base(getpid(), "0123456789"), &b);
}

static inline char *get_pid()
{
    return hash_map_get(hm, "$");
}

static inline void setup_random()
{
    bool b;
    hash_map_insert(hm, strdup("RANDOM"),
                    itoa_base((int)(rand() * (size_t)hm), "0123456789"), &b);
}

static inline char *get_random()
{
    char *out = hash_map_get(hm, "RANDOM");
    setup_random();
    return out;
}

static inline void setup_uid()
{
    bool b;
    hash_map_insert(hm, strdup("UID"), itoa_base(getuid(), "0123456789"), &b);
}

static inline char *get_uid()
{
    return hash_map_get(hm, "UID");
}

void setup_variables(int nb_args, char **args);

#endif /* ! VARIABLES_H*/
