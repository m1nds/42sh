#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ast/ast.h"
#include "exec/exec.h"

typedef int (*redirect_ptr)(void *);

int redirect_input(void *arg)
{
    struct ast *ast = (struct ast *)arg;
    int user_flags = S_IRUSR | S_IRGRP | S_IROTH;

    int file_fd = open(ast->value[0], O_RDONLY, user_flags);
    if (file_fd == -1)
    {
        return -1;
    }

    dup2(file_fd, STDIN_FILENO);

    return file_fd;
}

int redirect_output(void *arg)
{
    struct ast *ast = (struct ast *)arg;
    int user_flags = S_IWUSR | S_IWGRP | S_IWOTH;

    int file_fd = open(ast->value[0], O_WRONLY | O_CREAT, user_flags);
    if (file_fd == -1)
    {
        return -1;
    }

    dup2(file_fd, STDOUT_FILENO);

    return file_fd;
}

redirect_ptr match_redirect_func(enum ast_type type)
{
    static redirect_ptr buffer[] = {
        [NODE_REDIR_IN] = redirect_input, [NODE_REDIR_OUT] = redirect_output
    };

    return buffer[type];
}

size_t children_len(struct ast **children)
{
    size_t i = 0;
    while (*children)
    {
        children++;
        i++;
    }

    return i;
}

int handle_redirect(struct ast *ast)
{
    int save_stdin = dup(STDIN_FILENO);
    int save_stdout = dup(STDOUT_FILENO);

    size_t len = children_len(ast->children);

    struct ast **current = ast->children;
    struct ast *command = NULL;

    int *fds = malloc(len * sizeof(int));
    size_t i = 0;

    while (*current)
    {
        if ((*current)->node_type <= NODE_UNTIL)
        {
            command = *current;
        }
        else if ((*current)->node_type == NODE_ASSIGN)
        {
            // TODO: Need to implement variables.
        }
        else
        {
            redirect_ptr redir = match_redirect_func(ast->node_type);
            fds[i] = redir(*current);
            fcntl(fds[i], F_SETFD, FD_CLOEXEC);
            i++;
        }

        current++;
    }

    int out = (command != NULL) ? evaluate_ast(command) : 1;

    dup2(save_stdin, STDIN_FILENO);
    dup2(save_stdout, STDOUT_FILENO);

    for (size_t c = 0; c < i; c++)
        if (fds[c] != -1)
            close(fds[c]);

    free(fds);

    return out;
}
