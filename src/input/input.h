#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>

/**
 * @def: Used when input is on stdin
 */
static inline FILE *get_stdin()
{
    return stdin;
}

enum option_flags
{
    VERBOSE_FLAG = (1 << 0),
    PRETTY_PRINT_FLAG = (1 << 1)
};

/**
 * @def: Used when input is by a string
 * @args str: string passed in input with "-c" option
 */
FILE *get_string(char *str);

/**
 * @def: Used when a file is passed as input
 * @args file: path to the file as input
 */
FILE *get_file(const char *file);

/**
 * @def: Set the flags when options "--verbose" or "--pretty-print" are used
 * @args arg: string to compare with "--verbose" and "--pretty-print"
 * @args flags: the flags to change
 * @return : 1 if arg correspond to an option else 0
 */
int set_flags(const char *arg, int *flags);

/**
 * @def: check if the verbose option is ON
 */
int check_verbose(int *flags);

/**
 * @def: check if the pretty-print option is ON
 */
int check_prettyprint(int *flags);

/**
 * @def: Transform the input in a FILE* stream and returns it. (supports the 3
 * types of input)
 * @args nbargs: number of arguments passed to the executable, basically argc.
 * @args args: the arguments passed to the executable, basically argv.
 * @args flags: a pointer to an int, the first bit is set to 1 if the option
 * "--verbose" is within args, the second bit is set to 1 if the option
 * "--pretty-print" is within args.
 * @args err: in order to return the error code
 */
FILE *get_input(int nbargs, char **args, int *flags, int *err);

#endif /* ! INPUT_H */
