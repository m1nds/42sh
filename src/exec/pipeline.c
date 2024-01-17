#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "exec.h"

int exec_command(char **argv, int tmp_fds[2], int is_right)
{
    int pid = fork();
    if (pid == -1)
    {
        exit(1);
    }
    else if (!pid)
    {
        int fd_to_rep = is_right ? 0 : 1;
        int fd_end = is_right ? tmp_fds[0] : tmp_fds[1];

        if (dup2(fd_end, fd_to_rep) == -1)
        {
            exit(1);
        }

        close(tmp_fds[0]);
        close(tmp_fds[1]);

        if (execvp(argv[0], argv) == -1)
        {
            exit(1);
        }

        exit(1);
    }

    return pid;
}

int handle_pipe(struct ast *ast)
{
    int tmp_fds[2];
    if (pipe(tmp_fds) == -1)
    {
        return -1;
    }

    int left = exec_command(ast->children[0]->value, tmp_fds, 0);
    int right = exec_command(ast->children[1]->value, tmp_fds, 1);

    close(tmp_fds[0]);
    close(tmp_fds[1]);

    int out;
    waitpid(left, &out, 0);
    waitpid(right, &out, 0);

    return WEXITSTATUS(out);
}
