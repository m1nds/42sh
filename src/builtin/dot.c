#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtin/builtin.h"
#include "main_loop/main_loop.h"

static char *get_path(char *path)
{
    char *tok = strtok(path, ":");
    if (tok == NULL)
    {
        return NULL;
    }
    char *subpath = malloc(strlen(tok) + 2);
    strcpy(subpath, tok);
    if (subpath[strlen(subpath) - 1] != '/')
    {
        subpath = strcat(subpath, "/");
    }
    return subpath;
}

int dot(char **list_ptr)
{
    if (list_ptr[1] == NULL)
    {
        fprintf(stderr, ".: filename argument required\n");
        return 2;
    }

    size_t i = 0;
    while (list_ptr[1][i] != '\0')
    {
        if (list_ptr[1][i] == '/')
        {
            FILE *input = fopen(list_ptr[1], "r");
            return (input != NULL) ? main_execution_loop(input, 0) : 1;
        }
        i++;
    }

    char *path = getenv("PATH");
    char *subpath = get_path(path);

    FILE *input = NULL;
    while (subpath != NULL && input == NULL)
    {
        subpath = realloc(subpath, strlen(subpath) + strlen(list_ptr[1]) + 1);
        subpath = strcat(subpath, list_ptr[1]);

        input = fopen(subpath, "r");

        free(subpath);
        subpath = get_path(NULL);
    }
    free(subpath);
    return (input != NULL) ? main_execution_loop(input, 0) : 1;
}
