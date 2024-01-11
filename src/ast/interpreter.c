#include "ast/ast.h"

#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#include "builtin/builtin.h"

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
}
