#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils/vector.h"

const char *get_last_slash(struct vector *out)
{
    if (out->size == 0)
    {
        return NULL;
    }

    size_t i = out->size - 1;
    while (true)
    {
        if (out->data[i] == '/')
        {
            return out->data + i;
        }

        if (i == 0 && out)
        {
            break;
        }

        i--;
    }

    return NULL;
}

bool handle_dots(struct vector *out, const char *ptr, int len)
{
    int two = (len == 2) && (ptr[0] == '.' && ptr[1] == '.');
    int single = (len == 1) && (ptr[0] == '.');
    int zero = (len == 0);

    if (two)
    {
        const char *last_slash = get_last_slash(out);
        if (last_slash != NULL)
        {
            out->size = (size_t)(last_slash - out->data);
        }
        return 1;
    }

    return single || zero;
}

struct vector *canonical_path(struct vector *path)
{
    struct vector *can = vector_create(50);

    char *start = path->data;
    char *end = &(path->data[path->size]);

    char *next_slash = NULL;
    for (; start < end; start = next_slash + 1)
    {
        next_slash = memchr(start, '/', end - start);
        if (next_slash == NULL)
        {
            next_slash = end;
        }

        size_t len = next_slash - start;
        if (!handle_dots(can, start, len))
        {
            vector_append(can, '/');
            for (size_t i = 0; i < len; i++)
            {
                vector_append(can, start[i]);
            }
        }

        start = next_slash + 1;
    }

    vector_append(can, '\0');
    return can;
}

int handle_no_args(char *home, char *pwd_save)
{
    int out = 0;
    if (home != NULL && ((out = chdir(home)) != -1))
    {
        setenv("PWD", home, 1);
        setenv("OLDPWD", pwd_save, 1);
    }

    return (out != -1);
}

int handle_dash(char *pwd_save, char *oldpwd_save)
{
    int out = oldpwd_save != NULL;
    if (oldpwd_save != NULL && (out = chdir(oldpwd_save)) != -1)
    {
        setenv("PWD", oldpwd_save, 1);
        setenv("OLDPWD", pwd_save, 1);
    }

    if (out == -1)
    {
        fprintf(stderr, "OLDPWD is not set!\n");
    }
    else
    {
        fprintf(stdout, "%s\n", oldpwd_save);
    }

    return (out != -1);
}

int cd_builtin(char **args)
{
    char *home = getenv("HOME");
    char *pwd_save = getenv("PWD");
    char *oldpwd_save = getenv("OLDPWD");

    char *path = args[1];
    if (path == NULL)
    {
        return handle_no_args(home, pwd_save);
    }

    if (strcmp(path, "-") == 0)
    {
        return handle_dash(pwd_save, oldpwd_save);
    }

    struct vector *cp = vector_create(50);

    if (path[0] == '/')
    {
        vector_append_string(cp, path);
    }
    else
    {
        vector_append_string(cp, pwd_save);
        vector_append(cp, '/');
        vector_append_string(cp, path);
    }

    if (cp->size > 0 && cp->data[cp->size - 1] != '/')
        vector_append(cp, '/');

    struct vector *can = canonical_path(cp);

    int out = 0;
    if ((out = chdir(can->data)) != -1)
    {
        setenv("PWD", can->data, 1);
        setenv("OLDPWD", pwd_save, 1);
    }

    vector_destroy(can);
    vector_destroy(cp);

    if (out == -1)
        fprintf(stderr, "failed to change directory!\n");

    return (out != -1) ? 0 : 1;
}
