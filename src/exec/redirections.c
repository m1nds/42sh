#define _POSIX_C_SOURCE 200112L
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ast/ast.h"
#include "exec/exec.h"

typedef int (*redirect_ptr)(void *);

static int retrieve_fd(char *str)
{
    if (str == NULL)
    {
        return -1;
    }

    return atoi(str);
}

static int redirect_input(void *arg)
{
    struct ast *ast = (struct ast *)arg;
    int user_flags = 0644; /*S_IRUSR | S_IRGRP | S_IROTH*/
    int file_fd = open(ast->value[0], O_RDWR, user_flags);
    if (file_fd == -1)
    {
        return -1;
    }

    int io_number = retrieve_fd(ast->value[1]);
    int fd_to_switch = (io_number == -1) ? STDIN_FILENO : io_number;

    dup2(file_fd, fd_to_switch);

    return file_fd;
}

static int redirect_output(void *arg)
{
    struct ast *ast = (struct ast *)arg;
    int user_flags = 0644; /*S_IWUSR | S_IWGRP | S_IWOTH*/

    int file_fd = open(ast->value[0], O_RDWR | O_CREAT | O_TRUNC, user_flags);
    if (file_fd == -1)
    {
        return -1;
    }

    int io_number = retrieve_fd(ast->value[1]);
    int fd_to_switch = (io_number == -1) ? STDOUT_FILENO : io_number;

    dup2(file_fd, fd_to_switch);

    return file_fd;
}

static int redirect_input_and(void *arg)
{
    struct ast *ast = (struct ast *)arg;

    int dest = retrieve_fd(ast->value[0]);

    if (strcmp(ast->value[1], "-") == 0)
    {
        close(dest);
        return -1;
    }

    int src = retrieve_fd(ast->value[1]);
    int fd_to_switch = (dest == -1) ? STDIN_FILENO : dest;

    dup2(src, fd_to_switch);

    return -1;
}

static int redirect_output_and(void *arg)
{
    struct ast *ast = (struct ast *)arg;

    int dest = retrieve_fd(ast->value[0]);

    if (strcmp(ast->value[1], "-") == 0)
    {
        close(dest);
        return -1;
    }

    int src = retrieve_fd(ast->value[1]);
    int fd_to_switch = (dest == -1) ? STDOUT_FILENO : dest;

    dup2(src, fd_to_switch);

    return -1;
}

static int redirect_output_append(void *arg)
{
    struct ast *ast = (struct ast *)arg;
    int user_flags = S_IWUSR | S_IWGRP | S_IWOTH;

    int file_fd =
        open(ast->value[0], O_WRONLY | O_CREAT | O_APPEND, user_flags);
    if (file_fd == -1)
    {
        return -1;
    }

    int io_number = retrieve_fd(ast->value[1]);
    int fd_to_switch = (io_number == -1) ? STDOUT_FILENO : io_number;

    dup2(file_fd, fd_to_switch);

    return file_fd;
}

static void redirect_inout(struct ast *ast, int *fds, size_t *i)
{
    fds[(*i)++] = redirect_input(ast);
    fds[(*i)++] = redirect_output_append(ast);
    fcntl(fds[*i - 1], F_SETFD, FD_CLOEXEC);
    fcntl(fds[*i - 2], F_SETFD, FD_CLOEXEC);
}

redirect_ptr match_redirect_func(enum ast_type type)
{
    static redirect_ptr buffer[] = { [NODE_REDIR_IN] = redirect_input,
                                     [NODE_REDIR_OUT] = redirect_output,
                                     [NODE_REDIR_OUTA] = redirect_output_append,
                                     [NODE_REDIR_INAND] = redirect_input_and,
                                     [NODE_REDIR_OUTAND] =
                                         redirect_output_and };

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

    int *fds = malloc(2 * len * sizeof(int));
    size_t i = 0;

    while (*current)
    {
        if ((*current)->node_type <= NODE_UNTIL)
        {
            command = *current;
        }
        else if ((*current)->node_type == NODE_ASSIGN)
        {
            // Not great but it works for now
            if (len == 1)
            {
                setup_value((*current)->value[0], (*current)->value[1]);
            }
            else
            {
                setenv((*current)->value[0], (*current)->value[1], 1);
            }
        }
        else if ((*current)->node_type == NODE_REDIR_INOUT)
        {
            redirect_inout(*current, fds, &i);
        }
        else
        {
            redirect_ptr redir = match_redirect_func((*current)->node_type);
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
