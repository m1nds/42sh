#define _POSIX_C_SOURCE 200112L

#include <err.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "utils/hash_map.h"

extern struct hash_map *hm;

static int unset_var(char **list_str, size_t i)
{
    if (list_str[i] == NULL)
    {
        errx(1, "not enough arguments");
    }

    while (list_str[i] != NULL)
    {
        hash_map_remove(hm, list_str[i]);
        i++;
    }
    return 0;
}

static int unset_env(char **list_str, size_t i)
{
    if (list_str[i] == NULL)
    {
        errx(1, "not enough arguments");
    }

    while (list_str[i] != NULL)
    {
        unsetenv(list_str[i]);
        i++;
    }
    return 0;
}

static int unset_fun(char **list_str, size_t i)
{
    if (list_str[i])
    {
        return 1;
    }
    return 0;
}

int unset(char **list_str)
{
    if (list_str == NULL)
    {
        return 500000;
    }
    char flag_f = 0;
    char flag_v = 0;
    char break_flag = 0;
    size_t i = 0;
    while (list_str[i] != NULL)
    {
        size_t j = 0;
        char flag_option = 0;
        while (list_str[i][j] != '\0')
        {
            if (flag_option && list_str[i][j] == 'f')
            {
                flag_f = 1;
            }
            else if (flag_option && list_str[i][j] == 'v')
            {
                flag_v = 1;
            }
            else if (list_str[i][j] == '-')
            {
                if (flag_option || list_str[i][j + 1] == '\0')
                {
                    break_flag = 1;
                    break;
                }
                flag_option = 1;
            }
            else
            {
                break_flag = 1;
                break;
            }
            j++;
        }
        if (break_flag)
        {
            break;
        }
        i++;
    }
    if (flag_f && flag_v)
    {
        errx(1, "cannot simultaneously unset a function and a variable");
    }
    if (flag_f)
    {
        return unset_fun(list_str, i);
    }
    if (flag_v)
    {
        return unset_env(list_str, i);
    }
    return unset_var(list_str, i);
}
