#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "utils/hash_map.h"
#include "utils/vector.h"
#include "variables/variables.h"

struct eco_lines
{
    enum ast_type node_type;
    char *s;
};

static struct eco_lines init_eco(enum ast_type node_type, char *s)
{
    struct eco_lines eco;
    eco.node_type = node_type;
    eco.s = s;
    return eco;
}

static char is_continuous_name(char c, size_t len)
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

static char special_character(char c)
{
    if (c == '$' || c == '*' || c == '#' || c == '?' || c == '@'
        || (c >= '0' && c <= '9'))
    {
        return 1;
    }
    return 0;
}

static void preprocessing_brackets(char *string, struct vector *name, size_t *i)
{
    (*i)++;
    while (string[*i] != '}')
    {
        vector_append(name, string[*i]);
        (*i)++;
    }
}

static void preprocessing_single_quotes(char *string, struct vector *vec,
                                        size_t *i)
{
    (*i)++;
    while (string[*i] != '\'')
    {
        vector_append(vec, string[*i]);
        (*i)++;
    }
    (*i)++;
}

static void substitute_variable(char *string, struct vector *final_command,
                                struct vector **command_name, size_t *i)
{
    struct hash_map *hm_vars = get_variables();
    struct vector *name = *command_name;
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
    *command_name = name;
}

static void preprocessing_double_quotes(char *string,
                                        struct vector *final_command,
                                        struct vector **command_name, size_t *i)
{
    struct vector *name = *command_name;
    char name_flag = 0;
    char brackets_flag = 0;
    (*i)++;
    while (string[*i] != '"')
    {
        if (is_continuous_name(string[*i], strlen(name->data)) == 0
            && name_flag == 1)
        {
            substitute_variable(string, final_command, &name, i);
            name_flag = 0;
            if (string[*i] == '}' && brackets_flag == 1)
            {
                (*i)++;
            }
            brackets_flag = 0;
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
                brackets_flag = 1;
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
        substitute_variable(string, final_command, &name, i);
    }
    *command_name = name;
    (*i)++;
}

static void shift_strings(char **strings, size_t *i)
{
    size_t k = *i;
    for (; strings[k + 1] != NULL; k++)
    {
        strings[k] = strings[k + 1];
    }
    strings[k] = NULL;
    (*i)--;
}

static void add_character_to_vector(char c, struct vector *final_command,
                                    struct vector *name, char name_flag)
{
    if (name_flag == 0)
    {
        vector_append(final_command, c);
    }
    else
    {
        vector_append(name, c);
    }
}

static void end_process_string(char **strings, size_t *i,
                               struct vector *final_command, char quote_flag)
{
    free(strings[*i]);
    vector_append(final_command, '\0');
    if (strlen(final_command->data) == 0 && quote_flag == 0)
    {
        shift_strings(strings, i);
    }
    else
    {
        strings[*i] = strdup(final_command->data);
    }
}

static void reset_vectors(struct vector **final_command, struct vector **name)
{
    vector_destroy(*final_command);
    vector_destroy(*name);
    *final_command = vector_create(100);
    *name = vector_create(100);
}

static void destroy_vectors(struct vector *final_command, struct vector *name)
{
    vector_destroy(final_command);
    vector_destroy(name);
}

static void handle_double_quotes(struct eco_lines eco,
                                 struct vector *final_command,
                                 struct vector **name, size_t *j)
{
    if (eco.node_type == NODE_FOR)
        vector_append(final_command, '\"');
    preprocessing_double_quotes(eco.s, final_command, name, j);
    if (eco.node_type == NODE_FOR)
        vector_append(final_command, '\"');
}

static void preprocessing_strings(char **strings, struct vector *final_command,
                                  struct vector *name, enum ast_type node_type)
{
    size_t i = -1;
    while (strings[++i] != NULL)
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
                substitute_variable(strings[i], final_command, &name, &j);
                name_flag = 0;
                if (strings[i][j] == '}' && brackets_flag == 1)
                {
                    j++;
                }
                brackets_flag = 0;
            }
            else if (strings[i][j] == '$')
            {
                name_flag = 1;
                if (strings[i][++j] == '{')
                {
                    preprocessing_brackets(strings[i], name, &j);
                    brackets_flag = 1;
                }
            }
            else if (strings[i][j] == '\'')
            {
                preprocessing_single_quotes(strings[i], final_command, &j);
                quote_flag = 1;
            }
            else if (strings[i][j] == '"')
            {
                struct eco_lines eco = init_eco(node_type, strings[i]);
                handle_double_quotes(eco, final_command, &name, &j);
                quote_flag = 1;
            }
            else
            {
                if (strings[i][j] == '\\')
                {
                    j++;
                }
                add_character_to_vector(strings[i][j++], final_command, name,
                                        name_flag);
            }
        }
        vector_append(name, '\0');
        if (name_flag == 1)
        {
            substitute_variable(strings[i], final_command, &name, &j);
        }
        end_process_string(strings, &i, final_command, quote_flag);
        reset_vectors(&final_command, &name);
    }
    destroy_vectors(final_command, name);
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
            preprocessing_strings(ast->value + 1, final_command, name,
                                  ast->node_type);
        }
        else
        {
            preprocessing_strings(ast->value, final_command, name,
                                  ast->node_type);
        }
    }
}
