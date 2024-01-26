#include <stddef.h>
#include <stdlib.h>

int exit_builtin(char **list_str)
{
    int exit_code = 999;
    if (list_str[1] != NULL)
    {
        exit_code += atoi(list_str[1]) + 1;
    }
    return exit_code;
}
