#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <system_error>
#include <unistd.h>

#include "ast.h"
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
        node->value[1] = NULL;
    }
    else
    {
        node->value = NULL;
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
/*
int execute_command(struct ast *ast)
{
    if (ast->node_type != NODE_COMMAND)
    {
        return false_builtin();
    }

    if (strcmp(ast->content.command[0], "true") == 0)
    {
        return true_builtin();
    }

    if (strcmp(ast->content.command[0], "false") == 0)
    {
        return false_builtin();
    }

    if (strcmp(ast->content.command[0], "echo") == 0)
    {
        return echo_builtin(ast->content.command);
    }

    int pid = fork();
    if (pid == -1)
    {
        return false_builtin();
    }
    else if (!pid)
    {
        execvp(ast->content.command[0], ast->content.command);
    }

    int status;
    waitpid(pid, &status, 0);

    return WEXITSTATUS(status);
}

int handle_if(struct ast *ast)
{
    if (ast->node_type != NODE_IF)
    {
        return false_builtin();
    }

    if (evaluate_ast(ast->content.compound_list) == true_builtin())
    {
        return evaluate_ast(ast->left_child);
    }

    return evaluate_ast(ast->right_child);
}

int handle_semicolon(struct ast *ast)
{
    if (ast->node_type != NODE_SEMICOLON)
    {
        return false_builtin();
    }

    evaluate_ast(ast->left_child);
    return evaluate_ast(ast->right_child);
}

int evaluate_ast(struct ast *ast)
{
    if (ast == NULL)
    {
        return false_builtin();
    }

    switch (ast->node_type)
    {
        case NODE_COMMAND:
            //TODO: Execute a command
            return -1;
        case NODE_SEMICOLON:
            return handle_semicolon(ast);
        case NODE_IF:
            return handle_if(ast);
        default:
            return -1;
    }
}*/
