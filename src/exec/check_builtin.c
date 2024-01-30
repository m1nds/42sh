#include <string.h>

#include "builtin/builtin.h"
#include "exec/exec.h"

int check_builtin(char **value)
{
    if (value == NULL || value[0] == NULL)
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

    if (strcmp("unset", value[0]) == 0)
    {
        return unset(value);
    }

    if (strcmp(".", value[0]) == 0)
    {
        return dot(value);
    }

    if (strcmp("export", value[0]) == 0)
    {
        return export(value);
    }

    if (strcmp("exit", value[0]) == 0)
    {
        return exit_builtin(value);
    }

    if (strcmp("cd", value[0]) == 0)
    {
        return cd_builtin(value);
    }
    return -1;
}
