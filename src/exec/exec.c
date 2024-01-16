#include "exec.h"

#include "ast/ast.h"
/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>


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
*/
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
