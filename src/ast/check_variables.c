#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "utils/hash_map.h"

void replace_variables(struct ast *ast, struct hash_map *hm)
{
    if (ast == NULL)
    {
        return;
    }
    if (ast->node_type == NODE_COMMAND)
    {
        if (ast->value != NULL)
        {
            size_t i = 0;
            while (ast->value[i] != NULL)
            {
                if (ast->value[i][0] == '$')
                {
                    const char *val = hash_map_get(hm, ast->value[i] + 1);
                    if (val != NULL)
                    {
                        ast->value[i] = realloc(ast->value[i], strlen(val) + 1);
                        ast->value[i] = strcpy(ast->value[i], val);
                    }
                }
                i++;
            }
        }
    }
    if (ast->children != NULL)
    {
        size_t i = 0;
        while (ast->children[i] != NULL)
        {
            replace_variables(ast->children[i], hm);
            i++;
        }
    }
}
