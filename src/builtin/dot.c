#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int dot(char **list_ptr)
{
    if (list_ptr[1] == NULL)
    {
        return 1;
    }

    size_t i = 0;
    while (list_ptr[1][i] != '\0')
    {
        if (list_ptr[1][i] == '/')
        {
            char *const argv[] = {"./42sh", list_ptr[1], NULL}; 
            int is_exe = execvp("./42sh", argv);
            return is_exe;
        }
        i++;
    }
    char *path = getenv("PATH");
    char *tok = strtok(path, ":");
    char *subpath = malloc(strlen(tok) + 2);
    strcpy(subpath, tok);
    if (subpath[strlen(subpath) - 1] != '/')
    {
        subpath = strcat(subpath, "/");
    }
    while (subpath != NULL)
    {
        subpath = realloc(subpath, strlen(subpath) + strlen(list_ptr[1]) + 1);
        subpath = strcat(subpath, list_ptr[1]);
        char *const argv[] = {"42sh", subpath, NULL};
        int is_exe = execvp("42sh", argv);
        if (is_exe != -1)
        {
            return 2;
        }
        subpath = strtok(NULL, ":");
    }
    free(subpath);
    return -1;
}
