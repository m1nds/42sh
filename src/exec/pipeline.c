#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int close_pids(int *pids, int *builtins, size_t len)
{
    int out = -1;
    int builtin = 0;

    for (size_t i = 0; i < len; i++)
    {
        if (pids[i] != -1)
        {
            builtin = 0;
            waitpid(pids[i], &out, 0);
        }
        else
        {
            builtin = 1;
            out = builtins[i];
        }
    }

    free(pids);
    free(builtins);

    return (builtin) ? out : WEXITSTATUS(out);
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

int exec_command(struct ast *current, struct exec_frame *frame, int *pids,
                 int *builtin)
{
    int save_stdin = dup(STDIN_FILENO);
    int save_stdout = dup(STDOUT_FILENO);

    redirect(frame->mode, frame->p, frame->i);

    int check = check_builtin(current->value);
    int save_pid = -1;
    if (check == -1)
    {
        int pid = fork();
        if (pid == -1)
        {
            exit(-1);
        }
        else if (!pid)
        {
            close_pipes(frame->p, 1);
            free(pids);
            exit(evaluate_ast(current));
            /*if (execvp(current->value[0], current->value) == -1)
            {
                errx(127, "unknown command!\n");
            }*/
        }
        save_pid = pid;
    }
    else
    {
        *builtin = 1;
    }

    dup2(save_stdin, STDIN_FILENO);
    dup2(save_stdout, STDOUT_FILENO);

    close(save_stdin);
    close(save_stdout);

    return (check == -1) ? save_pid : check;
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

    int *builtins = calloc(len, sizeof(int));
    memset(builtins, -1, len * sizeof(int));

    int is_builtin = 0;

    struct exec_frame frame = { .mode = 0, .p = p, .i = 0 };
    current = ast->children;

    pids[0] = exec_command(*current, &frame, pids, &is_builtin);
    builtins[0] = (is_builtin) ? pids[0] : -1;

    current++;

    frame.mode = 1;
    frame.i = 1;
    while (*current && frame.i < len - 1)
    {
        pids[frame.i] = exec_command(*current, &frame, pids, &is_builtin);
        builtins[frame.i] = (is_builtin) ? pids[frame.i] : -1;

        frame.i++;
        current++;
    }

    frame.mode = 2;
    pids[frame.i] = exec_command(*current, &frame, pids, &is_builtin);
    builtins[frame.i] = (is_builtin) ? pids[frame.i] : -1;

    close_pipes(p, 1);
    int out = close_pids(pids, builtins, len);

    return out;
}
