#include <string.h>

#include "builtin/builtin.h"
#include "exec/exec.h"

int check_builtin(char **value)
{
    if (value == NULL)
    {
        return -1;
    }
    if (strcmp("echo", value[0]) == 0)
    {
        return echo_builtin(value);
    }

    if (strcmp("true", value[0]) == 0)
    {
        return true_builtin();
    }

    if (strcmp("false", value[0]) == 0)
    {
        return false_builtin();
    }

    return -1;
}
