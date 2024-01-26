#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "utils/hash_map.h"
#include "variables/variables.h"

int export(char **list_str)
{
    struct hash_map *hm_vars = get_variables();
    int err = 0;
    size_t i = 1;
    while (list_str[i] != NULL)
    {
        if (list_str[i][0] == '=')
        {
            return 1;
        }

        char *name = strtok(list_str[i], "=");
        char *val = strtok(NULL, "=");
        if (val == NULL)
        {
            val = hash_map_get(hm_vars, name);
            if (val != NULL)
            {
                err = setenv(name, val, 1);
                if (err == -1)
                {
                    return 1;
                }
            }
        }
        else
        {
            err = setenv(name, val, 1);
            if (err == -1)
            {
                return 1;
            }
        }
        i++;
    }
    return 0;
}
