#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "exec.h"

struct exec_frame
{
    int mode;
    size_t i;
    int **p;
};

void close_pipes(int **p, int cl)
{
    for (size_t i = 0; p[i]; i++)
    {
        if (cl)
        {
            close(p[i][0]);
            close(p[i][1]);
        }

        free(p[i]);
    }

    free(p);
}

int **allocate_pipes(size_t len)
{
    int **p = calloc(len, sizeof(int *));

    for (size_t i = 0; i < len - 1; i++)
        p[i] = calloc(2, sizeof(int));

    for (size_t i = 0; i < len - 1; i++)
    {
        if (pipe(p[i]) == -1)
        {
            close_pipes(p, 1);
            return NULL;
        }
    }
    return p;
}

int close_pids(int *pids, size_t len)
{
    int out = -1;
    for (size_t i = 0; i < len; i++)
    {
        waitpid(pids[i], &out, 0);
    }

    // free(pids);
    return out;
}

void redirect(int mode, int **p, int i)
{
    if (mode == 0)
    {
        dup2(p[i][1], STDOUT_FILENO);
        close(p[i][1]);
    }
    else if (mode == 1)
    {
        dup2(p[i - 1][0], STDIN_FILENO);
        dup2(p[i][1], STDOUT_FILENO);

        close(p[i - 1][0]);
        close(p[i][1]);
    }
    else
    {
        dup2(p[i - 1][0], STDIN_FILENO);
        close(p[i - 1][0]);
    }
}

int exec_command(struct ast *current, struct exec_frame *frame, int *pids)
{
    int save_stdin = dup(STDIN_FILENO);
    int save_stdout = dup(STDOUT_FILENO);

    redirect(frame->mode, frame->p, frame->i);

    int pid = fork();
    if (pid == -1)
    {
        exit(-1);
    }
    else if (!pid)
    {
        close_pipes(frame->p, 1);
        free(pids);

        int check = check_builtin(current->value);
        if (check != -1)
        {
            exit(check);
        }

        if (execvp(current->value[0], current->value) == -1)
        {
            errx(127, "unknown command!\n");
        }
    }

    dup2(save_stdin, STDIN_FILENO);
    dup2(save_stdout, STDOUT_FILENO);

    close(save_stdin);
    close(save_stdout);

    return pid;
}

int handle_pipe(struct ast *ast)
{
    struct ast **current = ast->children;
    size_t len = 0;

    while (*current)
    {
        len++;
        current++;
    }

    if (len <= 1)
    {
        return -1;
    }

    int **p = allocate_pipes(len);
    if (p == NULL)
    {
        return -1;
    }

    int *pids = calloc(len, sizeof(int));
    if (pids == NULL)
    {
        close_pipes(p, 0);
        return -1;
    }

    struct exec_frame frame = { .mode = 0, .p = p, .i = 0 };
    current = ast->children;

    pids[0] = exec_command(*current, &frame, pids);

    current++;

    frame.mode = 1;
    frame.i = 1;
    while (*current && frame.i < len - 1)
    {
        pids[frame.i] = exec_command(*current, &frame, pids);
        frame.i++;
        current++;
    }

    frame.mode = 2;
    pids[frame.i] = exec_command(*current, &frame, pids);

    close_pipes(p, 1);
    int out = close_pids(pids, len);

    return WEXITSTATUS(out);
}
