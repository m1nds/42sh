#define _POSIX_C_SOURCE 200809L

#include "input.h"

#include <err.h>
#include <stdio.h>
#include <string.h>

FILE *get_string(char *str)
{
    // Open a stream with a string str and returns it.
    return fmemopen(str, strlen(str), "r+");
}

FILE *get_file(const char *file)
{
    return fopen(file, "r+");
}

int set_flags(const char *arg, int *flags)
{
    if (strcmp(arg, "--verbose") == 0)
    {
        // Set the first bit to 1.
        *flags |= VERBOSE_FLAG;
        return 1;
    }
    if (strcmp(arg, "--pretty-print") == 0)
    {
        // Set the second bit to 1.
        *flags |= PRETTY_PRINT_FLAG;
        return 1;
    }
    return 0;
}

int check_verbose(int *flags)
{
    // Compare the flags with the verbose enum
    return *flags & VERBOSE_FLAG;
}

int check_prettyprint(int *flags)
{
    // Compare the flags with the pretty-print enum
    return *flags & PRETTY_PRINT_FLAG;
}

FILE *get_input(int nbargs, char **args, int *flags)
{
    // Iterate over all arguments (skip the first one "./42sh")
    for (int i = 1; i < nbargs; i++)
    {
        // Set the flags and continue if "--verbose" or "--pretty-print"
        int skip = set_flags(args[i], flags);
        if (skip)
        {
            continue;
        }
        // Input by string
        if (strcmp(args[i], "-c") == 0)
        {
            // check if "-c" is the last argument
            if (i + 1 < nbargs)
            {
                return get_string(args[i + 1]);
            }
            else
            {
                // Set error code to 2 because "-c" option requires an argument
                errx(2, "option requires an argument");
                return NULL;
            }
        }
        // Input by file
        FILE *fp = get_file(args[i]);
        if (!fp)
        {
            // Set error code to 127 because the file doesn't exist.
            errx(127, "No such file or directory");
        }
        return fp;
    }
    // Input on stdin
    return get_stdin();
}
