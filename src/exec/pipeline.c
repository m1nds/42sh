#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "exec.h"

void close_fds(int **p, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        if (p[i][0] != STDOUT_FILENO && p[i][0] != STDIN_FILENO)
        {
            close(p[i][0]);
        }

        if (p[i][1] != STDOUT_FILENO && p[i][1] != STDIN_FILENO)
        {
            close(p[i][1]);
        }
    }
}

int exec_command(struct ast *current, int pos, int **p, int i)
{
    int pid = fork();
    if (pid == -1)
    {
        return -1;
    }
    else if (!pid)
    {
        if (pos == 0)
        {
            dup2(p[i][1], STDOUT_FILENO);
        }
        else if (pos == 1)
        {
            dup2(p[i - 1][0], STDIN_FILENO);
            dup2(p[i][1], STDOUT_FILENO);
        }
        else
        {
            dup2(p[i][0], STDIN_FILENO);
        }

        if (execvp(current->value[0], current->value) == -1)
        {
            exit(127);
        }
    }

    int out;
    waitpid(pid, &out, 0);

    return WEXITSTATUS(out);
}

int handle_pipe(struct ast *ast)
{
    if (ast == NULL)
    {
        return -1;
    }

    struct ast **current = ast->children;
    size_t len = 0;

    while (*current)
    {
        len++;
        current++;
    }

    if (len <= 1)
    {
        return 0;
    }

    int **p = malloc((len - 1) * sizeof(int *));
    for (size_t i = 0; i < len - 1; i++)
        p[i] = calloc(2, sizeof(int));

    for (size_t i = 0; i < len; i++)
    {
        if (pipe(p[i]) == -1)
        {
            close_fds(p, i);
            free(p);
            return -1;
        }
    }

    current = ast->children;
    exec_command(*current, 0, p, 0);

    current++;
    size_t i = 1;
    while (*current)
    {
        exec_command(*current, 1, p, i);
        i++;
        current++;
    }

    int out = exec_command(*current, 1, p, i);
    close_fds(p, len - 1);

    for (size_t i = 0; i < len - 1; i++)
        free(p[0]);

    free(p);

    return out;
}
