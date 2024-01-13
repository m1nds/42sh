#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast/ast.h"
#include "builtin/builtin.h"

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

void print_tabs(size_t tabs)
{
    for (size_t i = 0; i < tabs; i++)
    {
        printf("    ");
    }
}

void true_print_ast(struct ast *ast, size_t tabs)
{
    if (ast == NULL)
    {
        return;
    }
    printf("Type: ");
    switch (ast->node_type)
    {
    case NODE_COMMAND:
        printf("Command: ");
        break;
    case NODE_IF:
        printf("If\n");
        break;
    case NODE_SEMICOLON:
        printf(";\n");
        break;
    }
    if (ast->value != NULL)
    {
        size_t i = 0;
        while (ast->value[i] != NULL)
        {
            printf("%s ", ast->value[i]);
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

int check_builtin(char **value)
{
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

int handle_command(struct ast *ast)
{
    if (ast->node_type != NODE_COMMAND)
    {
        return -1;
    }

    if (ast->value == NULL)
    {
        return 0;
    }

    int cb = check_builtin(ast->value);
    if (cb != -1)
    {
        return cb;
    }

    int pid = fork();
    if (pid == -1)
    {
        return -1;
    }
    else if (!pid)
    {
        if (execvp(ast->value[0], ast->value) == -1)
        {
            exit(127);
        }
    }

    int status;
    waitpid(pid, &status, 0);

    return WEXITSTATUS(status);
}

int handle_semicolon(struct ast *ast)
{
    if (ast->node_type != NODE_SEMICOLON)
    {
        return -1;
    }

    struct ast **current_child = ast->children;
    int out = 0;

    while (*current_child)
    {
        out = evaluate_ast(*current_child);
        current_child++;
    }

    return out;
}

int handle_if(struct ast *ast)
{
    if (ast->node_type != NODE_IF)
    {
        return -1;
    }

    int condition = evaluate_ast(ast->children[0]);
    if (condition == true_builtin())
    {
        return evaluate_ast(ast->children[1]);
    }

    return (ast->children[2] != NULL) ? evaluate_ast(ast->children[2]) : 0;
}

int evaluate_ast(struct ast *ast)
{
    if (ast == NULL)
    {
        return 0;
    }

    switch (ast->node_type)
    {
    case NODE_COMMAND:
        return handle_command(ast);
    case NODE_SEMICOLON:
        return handle_semicolon(ast);
    case NODE_IF:
        return handle_if(ast);
    default:
        return -1;
    }
}
