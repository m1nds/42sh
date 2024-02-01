#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "exec.h"
#include "exec/exec.h"
#include "variables/preprocessing.h"
#include "variables/variables.h"

static struct ast *deep_copy_ast(struct ast *ast)
{
    if (ast == NULL)
        return NULL;
    char **value = NULL;
    if (ast->value != NULL)
    {
        for (size_t i = 0; ast->value[i] != NULL; i++)
        {
            value = realloc(value, sizeof(char *) * (i + 2));
            value[i] = calloc(strlen(ast->value[i]) + 1, sizeof(char));
            value[i] = strcpy(value[i], ast->value[i]);
            value[i + 1] = NULL;
        }
    }
    struct ast *ret = ast_new(ast->node_type, 0, NULL);
    ret->value = value;
    if (ast->children != NULL)
    {
        for (size_t i = 0; ast->children[i] != NULL; i++)
        {
            ret->children =
                realloc(ret->children, sizeof(struct ast *) * (i + 2));
            ret->children[i] = deep_copy_ast(ast->children[i]);
            ret->children[i + 1] = NULL;
        }
    }
    return ret;
}

int handle_while(struct ast *ast)
{
    if (ast->node_type != NODE_WHILE)
    {
        return -1;
    }
    struct ast *tmp = deep_copy_ast(ast->children[0]);
    int condition = evaluate_ast(tmp);
    ast_free(tmp);
    if (condition >= 999)
    {
        return condition;
    }
    int retour = 0;
    while (condition == 0)
    {
        tmp = deep_copy_ast(ast->children[1]);
        retour = evaluate_ast(tmp);
        ast_free(tmp);
        if (retour >= 999)
        {
            return retour;
        }
        tmp = deep_copy_ast(ast->children[0]);
        condition = evaluate_ast(tmp);
        ast_free(tmp);
        if (condition >= 999)
        {
            return condition;
        }
    }

    return retour;
}

int handle_until(struct ast *ast)
{
    if (ast->node_type != NODE_UNTIL)
    {
        return -1;
    }
    struct ast *tmp = deep_copy_ast(ast->children[0]);
    int condition = evaluate_ast(tmp);
    ast_free(tmp);
    if (condition >= 999)
    {
        return condition;
    }
    int retour = 0;
    while (condition != 0)
    {
        tmp = deep_copy_ast(ast->children[1]);
        retour = evaluate_ast(tmp);
        ast_free(tmp);
        if (retour >= 999)
        {
            return retour;
        }
        tmp = deep_copy_ast(ast->children[0]);
        condition = evaluate_ast(tmp);
        ast_free(tmp);
        if (condition >= 999)
        {
            return condition;
        }
    }

    return retour;
}

static int mini_lexing(char **value, char ***arr)
{
    size_t i1 = 1;
    size_t k1 = 0;
    size_t i2 = 0;
    size_t k2 = 0;
    char flag = 0;
    (*arr) = calloc(2, sizeof(char *));
    while (value[i1] != NULL)
    {
        if (value[i1][k1] == '"')
        {
            flag = !flag;
            k1++;
        }
        else if (value[i1][k1] == '\0')
        {
            if ((*arr)[i2] != NULL)
            {
                (*arr)[i2][k2] = '\0';
                (*arr) = realloc((*arr), sizeof(char **) * (i2 + 2));
                i2++;
                (*arr)[i2] = NULL;
            }
            i1++;
            k1 = 0;
            k2 = 0;
        }
        else if (!flag
                 && (value[i1][k1] == '\t' || value[i1][k1] == '\n'
                     || value[i1][k1] == ' '))
        {
            if ((*arr)[i2] != NULL)
            {
                (*arr)[i2][k2] = '\0';
                (*arr) = realloc((*arr), sizeof(char **) * (i2 + 2));
                i2++;
                (*arr)[i2] = NULL;
            }
            k2 = 0;
            k1++;
        }
        else
        {
            (*arr)[i2] = realloc((*arr)[i2], sizeof(char) * (k2 + 2));
            (*arr)[i2][k2] = value[i1][k1];
            k2++;
            k1++;
        }
    }
    return i2;
}

static void free_all_arg(char **arr)
{
    for (int i = 0; arr[i] != NULL; i++)
    {
        free(arr[i]);
    }
    free(arr);
}

int handle_for(struct ast *ast)
{
    if (ast->node_type != NODE_FOR)
    {
        return -1;
    }
    int r = 0;
    replace_variables(ast);
    size_t i = 0;
    char **arr = NULL;
    mini_lexing(ast->value, &arr);
    while (arr[i] != NULL)
    {
        fflush(stdout);
        setup_value(strdup(ast->value[0]), strdup(arr[i]));
        struct ast *tmp = deep_copy_ast(ast->children[0]);
        r = evaluate_ast(tmp);
        ast_free(tmp);
        if (r >= 999)
        {
            free_all_arg(arr);
            return r;
        }
        i++;
    }
    free_all_arg(arr);
    return r;
}
