#define _POSIX_C_SOURCE 200809L

#include "input.h"

#include <stdio.h>
#include <string.h>

FILE *get_string(char *str)
{
    return fmemopen(str, strlen(str), "r");
}

FILE *get_file(const char *file)
{
    return fopen(file, "r");
}

int set_flags(const char *arg, int *flags)
{
    if (strcmp(arg, "--verbose") == 0)
    {
        *flags |= 1 << 0;
        return 1;
    }
    if (strcmp(arg, "--pretty-print") == 0)
    {
        *flags |= 1 << 1;
        return 1;
    }
    return 0;
}

FILE *parse_input(int nbargs, char **args, int *flags)
{
    for (int i = 1; i < nbargs; i++)
    {
        int skip = set_flags(args[i], flags);
        if (skip)
        {
            continue;
        }
        if (strcmp(args[i], "-c") == 0) // Input by string
        {
            if (i + 1 < nbargs)
            {
                return get_string(args[i + 1]);
            }
        }
        // Input by file
        return get_file(args[i]);
    }
    // Input on stdin
    return get_stdin();
}

/*int main(int argc, char **argv)
{
    int flags = 0;
    FILE *fd = parse_input(argc, argv, &flags, NULL);
    if (!fd)
    {
        printf("input error\n");
        return 1;
    }
    char buf[1];
    ssize_t r = fread(buf, 1, 1, fd);
    while (r)
    {
        printf("%c", buf[0]);
        r = fread(buf, 1, 1, fd);
    }
    printf("\nflags: %d\n", flags);
}*/
