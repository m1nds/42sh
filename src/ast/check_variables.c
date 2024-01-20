#define _POSIX_C_SOURCE 200809L
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "utils/hash_map.h"
#include "utils/vector.h"

extern struct hash_map *hm;

char is_continuous_name(char c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
        || (c >= '0' && c <= '9') || c == '_')
    {
        return 1;
    }
    return 0;
}

char special_character(char c)
{
    if (c == '$' || c == '*' || c == '#' || c == '?' || c == '@')
    {
        return 1;
    }
    return 0;
}

void replace_variables(struct ast *ast)
{
    if (ast == NULL)
    {
        return;
    }
    struct vector *final_command = vector_create(100);
    struct vector *name = vector_create(100);
    if (ast->node_type == NODE_COMMAND)
    {
        if (ast->value != NULL)
        {
            size_t i = 0;
            while (ast->value[i] != NULL)
            {
                size_t j = 0;
                char name_flag = 0;
                while (ast->value[i][j] != '\0')
                {
                    if (is_continuous_name(ast->value[i][j]) == 0
                        && name_flag == 1)
                    {
                        if (name_flag == 1)
                        {
                            if (strlen(name->data) == 0)
                            {
                                if (special_character(ast->value[i][j]) == 1)
                                {
                                    vector_append(name, ast->value[i][j]);
                                }
                                else
                                {
                                    vector_append(final_command, '$');
                                    vector_append(final_command,
                                                  ast->value[i][j]);
                                }
                            }
                            char *val = hash_map_get(hm, name->data);
                            if (val != NULL)
                            {
                                vector_append_string(final_command, val);
                            }
                        }
                        vector_destroy(name);
                        name = vector_create(100);
                        name_flag = 0;
                    }
                    else if (ast->value[i][j] == '$')
                    {
                        name_flag = 1;
                    }
                    else
                    {
                        if (name_flag == 0)
                        {
                            vector_append(final_command, ast->value[i][j]);
                        }
                        else
                        {
                            vector_append(name, ast->value[i][j]);
                        }
                    }
                    j++;
                }
                char *val = hash_map_get(hm, name->data);
                if (val != NULL)
                {
                    vector_append_string(final_command, val);
                }
                free(ast->value[i]);
                ast->value[i] = strdup(final_command->data);
                vector_destroy(final_command);
                vector_destroy(name);
                final_command = vector_create(100);
                name = vector_create(100);
                i++;
            }
        }
    }
    if (ast->children != NULL)
    {
        size_t i = 0;
        while (ast->children[i] != NULL)
        {
            replace_variables(ast->children[i]);
            i++;
        }
    }
}
