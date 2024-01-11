#include <stdio.h>

#include "builtin.h"

int echo_builtin(const char *str)
{
    puts(str);
    return true_builtin();
}
