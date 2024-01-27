#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "utils/hash_map.h"
#include "utils/vector.h"
#include "variables/variables.h"

char is_continuous_name(char c, size_t len)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
    {
        return 1;
    }
    if ((c >= '0' && c <= '9') && len > 0)
    {
        return 1;
    }
    return 0;
}

char special_character(char c)
{
    if (c == '$' || c == '*' || c == '#' || c == '?' || c == '@'
        || (c >= '0' && c <= '9'))
    {
        return 1;
    }
    return 0;
}

void preprocessing_brackets(char *string, struct vector *name, size_t *i)
{
    (*i)++;
    while (string[*i] != '}')
    {
        vector_append(name, string[*i]);
        (*i)++;
    }
}

void preprocessing_single_quotes(char *string, struct vector *vec, size_t *i)
{
    (*i)++;
    while (string[*i] != '\'')
    {
        vector_append(vec, string[*i]);
        (*i)++;
    }
    (*i)++;
}

void preprocessing_double_quotes(char *string, struct vector *final_command,
                                 struct vector **command_name, size_t *i)
{
    struct hash_map *hm_vars = get_variables();
    struct vector *name = *command_name;
    char name_flag = 0;
    (*i)++;
    while (string[*i] != '"')
    {
        if (is_continuous_name(string[*i], strlen(name->data)) == 0
            && name_flag == 1)
        {
            if (strlen(name->data) == 0)
            {
                if (special_character(string[*i]) == 1)
                {
                    vector_append(name, string[(*i)++]);
                }
                else
                {
                    vector_append(final_command, '$');
                }
            }
            char *val = NULL;
            if (strcmp(name->data, "RANDOM") == 0)
            {
                val = get_random(name->data);
            }
            else
            {
                val = hash_map_get(hm_vars, name->data);
            }
            if (val == NULL)
            {
                val = getenv(name->data);
            }
            if (val != NULL)
            {
                vector_append_string(final_command, val);
            }
            vector_destroy(name);
            name = vector_create(100);
            name_flag = 0;
            if (string[*i] == '}')
            {
                (*i)++;
            }
            continue;
        }
        else if (string[*i] == '\\')
        {
            (*i)++;
        }
        else if (string[*i] == '$')
        {
            name_flag = 1;
            (*i)++;
            if (string[*i] == '{')
            {
                preprocessing_brackets(string, name, i);
            }
            continue;
        }
        if (name_flag == 0)
        {
            vector_append(final_command, string[*i]);
        }
        else
        {
            vector_append(name, string[*i]);
        }
        (*i)++;
    }
    if (name_flag == 1)
    {
        char *val = NULL;
        if (strcmp(name->data, "RANDOM") == 0)
        {
            val = get_random(name->data);
        }
        else
        {
            val = hash_map_get(hm_vars, name->data);
        }
        if (val == NULL)
        {
            val = getenv(name->data);
        }
        if (val != NULL)
        {
            vector_append_string(final_command, val);
        }
        vector_destroy(name);
        name = vector_create(100);
    }
    *command_name = name;
    (*i)++;
}

void preprocessing_strings(char **strings, struct vector *final_command,
                           struct vector *name)
{
    struct hash_map *hm_vars = get_variables();
    size_t i = 0;
    while (strings[i] != NULL)
    {
        size_t j = 0;
        char name_flag = 0;
        char brackets_flag = 0;
        char quote_flag = 0;
        while (strings[i][j] != '\0')
        {
            if (is_continuous_name(strings[i][j], strlen(name->data)) == 0
                && name_flag == 1)
            {
                if (strlen(name->data) == 0)
                {
                    if (special_character(strings[i][j]) == 1)
                    {
                        vector_append(name, strings[i][j++]);
                    }
                    else
                    {
                        vector_append(final_command, '$');
                    }
                }
                vector_append(name, '\0');
                char *val = NULL;
                if (strcmp(name->data, "RANDOM") == 0)
                {
                    val = get_random(name->data);
                }
                else
                {
                    val = hash_map_get(hm_vars, name->data);
                }
                if (val == NULL)
                {
                    val = getenv(name->data);
                }
                if (val != NULL)
                {
                    vector_append_string(final_command, val);
                }
                vector_destroy(name);
                name = vector_create(100);
                name_flag = 0;
                if (strings[i][j] == '}' && brackets_flag == 1)
                {
                    j++;
                }
                brackets_flag = 0;
                continue;
            }
            else if (strings[i][j] == '$')
            {
                name_flag = 1;
                j++;
                if (strings[i][j] == '{')
                {
                    preprocessing_brackets(strings[i], name, &j);
                    brackets_flag = 1;
                }
                continue;
            }
            else if (strings[i][j] == '\'')
            {
                preprocessing_single_quotes(strings[i], final_command, &j);
                quote_flag = 1;
                continue;
            }
            else if (strings[i][j] == '"')
            {
                preprocessing_double_quotes(strings[i], final_command, &name,
                                            &j);
                quote_flag = 1;
                continue;
            }
            else
            {
                if (strings[i][j] == '\\')
                {
                    j++;
                }
                if (name_flag == 0)
                {
                    vector_append(final_command, strings[i][j]);
                }
                else
                {
                    vector_append(name, strings[i][j]);
                }
            }
            j++;
        }
        vector_append(name, '\0');
        char *val = NULL;
        if (strcmp(name->data, "RANDOM") == 0)
        {
            val = get_random(name->data);
        }
        else
        {
            val = hash_map_get(hm_vars, name->data);
        }
        if (val == NULL)
        {
            val = getenv(name->data);
        }
        if (val != NULL)
        {
            vector_append_string(final_command, val);
        }
        free(strings[i]);
        vector_append(final_command, '\0');
        if (strlen(final_command->data) == 0 && quote_flag == 0)
        {
            size_t j = i;
            for (; strings[j + 1] != NULL; j++)
            {
                strings[j] = strings[j + 1];
            }
            strings[j] = NULL;
            i--;
        }
        else
        {
            strings[i] = strdup(final_command->data);
        }
        vector_destroy(final_command);
        vector_destroy(name);
        final_command = vector_create(100);
        name = vector_create(100);
        i++;
    }
    vector_destroy(final_command);
    vector_destroy(name);
}
// Replace function name by preprocessing
void replace_variables(struct ast *ast)
{
    if (ast == NULL)
    {
        return;
    }
    struct vector *final_command = vector_create(100);
    struct vector *name = vector_create(100);
    if (ast->value != NULL)
    {
        if (ast->node_type == NODE_ASSIGN)
        {
            preprocessing_strings(ast->value + 1, final_command, name);
        }
        else
        {
            preprocessing_strings(ast->value, final_command, name);
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
