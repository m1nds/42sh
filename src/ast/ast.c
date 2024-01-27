#include "ast/ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtin/builtin.h"

/*
 * @def:    An array containing the string associated with an enum ast_type
 *          at the ast_type index
 * */
char *token_to_string[] = { "Command: ", ";\n",       "If\n",  "|\n",
                            "!\n",       "&&\n",      "||\n",  "While\n",
                            "Until\n",   "Prefixs\n", "< ",    "> ",
                            ">> ",       "<& ",       ">& ",   "<> ",
                            "Assign ",   "Function ", "For\n", "Subshell\n" };

struct ast *ast_new(enum ast_type type, size_t nb_child, char *value)
{
    struct ast *node = calloc(1, sizeof(struct ast));

    // malloc checking
    if (node == NULL)
    {
        return NULL;
    }

    // if nb_child == 0 then ast->children = NULL else ast->children NULL T
    if (nb_child != 0)
    {
        node->children = calloc(nb_child + 1, sizeof(struct ast));

        // malloc checking
        if (node->children == NULL)
        {
            free(node);
            return NULL;
        }
    }

    // if value == NULL then ast->value = NULL else ast->value NULL T
    if (value != NULL)
    {
        // on ast_new one string is given if it is a command so it's a list of 2
        node->value = calloc(2, sizeof(char *));

        // malloc checking
        if (node->value == NULL)
        {
            free(node->children);
            free(node);
            return NULL;
        }
        node->value[0] = value;
    }
    node->node_type = type;
    return node;
}

void ast_free(struct ast *ast)
{
    // protection
    if (ast == NULL)
        return;

    // free the list of value
    if (ast->value != NULL)
    {
        size_t i = 0;
        while (ast->value[i] != NULL)
        {
            free(ast->value[i]);
            i++;
        }
        free(ast->value);
    }

    // free the list of child
    if (ast->children != NULL)
    {
        size_t i = 0;
        while (ast->children[i] != NULL)
        {
            // recursive iteration
            ast_free(ast->children[i]);
            i++;
        }
        free(ast->children);
    }
    free(ast);
}

struct ast *ast_deep_copy(struct ast *ast)
{
    if (ast == NULL)
    {
        return NULL;
    }
    struct ast *copy = ast_new(ast->node_type, 0, NULL);
    size_t i = 0;
    if (ast->children != NULL)
    {
        while (ast->children[i] != NULL)
        {
            copy->children =
                realloc(copy->children, sizeof(struct ast) * (i + 2));
            copy->children[i] = ast_deep_copy(ast->children[i]);
            i++;
        }
        copy->children[i] = NULL;
    }
    i = 0;
    if (ast->value != NULL)
    {
        while (ast->value[i] != NULL)
        {
            copy->value = realloc(copy->value, sizeof(char *) * (i + 2));
            copy->value[i] = strdup(ast->value[i]);
            i++;
        }
        copy->value[i] = NULL;
    }
    return copy;
}

static void print_tabs(size_t tabs)
{
    for (size_t i = 0; i < tabs; i++)
    {
        printf("    ");
    }
}

static void true_print_ast(struct ast *ast, size_t tabs)
{
    if (ast == NULL)
    {
        return;
    }
    printf("Type: %s", token_to_string[ast->node_type]);
    if (ast->value != NULL)
    {
        size_t i = 0;
        while (ast->value[i] != NULL)
        {
            printf("|%s| ", ast->value[i]);
            i++;
        }
        printf("\n");
    }
    if (ast->children != NULL)
    {
        print_tabs(tabs);
        printf("{\n");
        size_t i = 0;
        while (ast->children[i] != NULL)
        {
            print_tabs(tabs + 1);
            printf("Child %lu: ", i);
            true_print_ast(ast->children[i], tabs + 1);
            i++;
        }
        print_tabs(tabs);
        printf("}\n");
    }
}

void print_ast(struct ast *ast)
{
    true_print_ast(ast, 0);
}
