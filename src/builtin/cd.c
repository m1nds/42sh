#define _POSIX_C_SOURCE 200112L
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include "builtin/builtin.h"
#include "utils/vector.h"

struct vector *rearrange_doubledots(struct vector *cr)
{
    size_t s = cr->size;
    struct vector *new_path = vector_create(100);
    vector_append(new_path, cr->data[0]);

    size_t prev = 1;

    for (size_t i = 1; i < s; i++)
    {
        vector_append(new_path, cr->data[i]);

        if (cr->data[i - 1] == '.' && cr->data[i] == '.')
        {
            for (size_t c = start; c < end; c++)
            {
                vector_pop(new_path);
            }
        }
        else if (cr->data[i] == '/')
        {

        }
    }

    vector_append(new_path, '\0');

    return new_path;
}

int cd_builtin(char **args)
{
    char *current_home = getenv("HOME");

    // If there are no args, check if $HOME exists and change $PWD
    if (args[0] == NULL)
    {
        return (current_home != NULL) ? setenv("PWD", current_home, 1) : 0;
    }

    struct vector *currpath = vector_create(100);
    if (args[0][0] == '/')
    {
        vector_append_string(currpath, args[0]);
    }

    struct vector *new_path = rearrange_doubledots(currpath);
    printf("%s\n", new_path->data);

    vector_destroy(currpath);
    vector_destroy(new_path);
    return 0;
}
