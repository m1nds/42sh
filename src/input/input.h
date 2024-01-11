#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>

static inline FILE *get_stdin()
{
    return stdin;
}

FILE *get_string(char *str);

FILE *get_file(const char *file);

FILE *parse_input(int nbargs, char **args, int *flags);

#endif /* ! INPUT_H */
