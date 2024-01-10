#include "builtin.h"

#include <stdio.h>

int echo_builtin(const char *str)
{
    puts(str);
    return true_builtin();
}
