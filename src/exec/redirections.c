#define _POSIX_C_SOURCE 200809L
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ast/ast.h"
#include "exec/exec.h"
#include "variables/preprocessing.h"
#include "variables/variables.h"

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

    int dest = retrieve_fd(ast->value[1]);

    if (strcmp(ast->value[1], "-") == 0)
    {
        close(dest);
        return -1;
    }

    int src = retrieve_fd(ast->value[0]);
    int fd_to_switch = (dest == -1) ? STDIN_FILENO : dest;

    dup2(src, fd_to_switch);

    return -1;
}

static int redirect_output_and(void *arg)
{
    struct ast *ast = (struct ast *)arg;

    int dest = retrieve_fd(ast->value[1]);

    if (strcmp(ast->value[1], "-") == 0)
    {
        close(dest);
        return -1;
    }

    int src = retrieve_fd(ast->value[0]);
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

bool no_command(struct ast **children)
{
    size_t i = 0;
    while (children[i] != NULL)
    {
        if (children[i]->node_type == NODE_COMMAND)
        {
            return false;
        }
        i++;
    }
    return true;
}

void preprocess_command(struct ast **children)
{
    size_t i = 0;
    while (children[i] != NULL)
    {
        if (children[i]->node_type == NODE_COMMAND)
        {
            replace_variables(children[i]);
            return;
        }
        i++;
    }
}

void handle_assign(struct ast *ast, char ***env_variables,
                   size_t *nb_env_variables, bool no_commands)
{
    char **environment_variables = *env_variables;
    size_t nb_environment_variables = *nb_env_variables;
    replace_variables(ast);
    // Handles the case where the variable is set to nothing
    if (ast->value[1] == NULL)
    {
        ast->value[1] = strdup("");
    }
    char *key = ast->value[0];
    char *value = ast->value[1];
    if (no_commands)
    {
        setup_value(strdup(key), strdup(value));
    }
    else
    {
        nb_environment_variables += 2;
        environment_variables =
            realloc(environment_variables,
                    sizeof(char *) * (nb_environment_variables + 1));
        environment_variables[nb_environment_variables - 2] = key;
        environment_variables[nb_environment_variables - 1] = getenv(key);
        environment_variables[nb_environment_variables] = NULL;
        setenv(key, value, 1);
    }
    *env_variables = environment_variables;
    *nb_env_variables = nb_environment_variables;
}

void reset_env_variables(char **env_variables)
{
    if (env_variables == NULL)
    {
        return;
    }
    size_t i = 0;
    while (env_variables[i] != NULL /* && env_variables[i + 1] != NULL*/)
    {
        char *key = env_variables[i];
        char *value = env_variables[i + 1];
        if (value == NULL)
        {
            unsetenv(key);
        }
        else
        {
            setenv(key, value, 1);
        }
        i += 2;
    }
}

int handle_redirect(struct ast *ast)
{
    int save_stdin = dup(STDIN_FILENO);
    int save_stdout = dup(STDOUT_FILENO);

    size_t len = children_len(ast->children);
    bool no_commands = no_command(ast->children);
    if (no_commands == false)
    {
        preprocess_command(ast->children);
    }
    struct ast **current = ast->children;
    struct ast *command = NULL;
    char **env_variables = NULL;
    size_t nb_env_variables = 0;
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
            handle_assign(*current, &env_variables, &nb_env_variables,
                          no_commands);
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
    int out = (command != NULL) ? handle_command(command, false) : 0;

    dup2(save_stdin, STDIN_FILENO);
    dup2(save_stdout, STDOUT_FILENO);
    reset_env_variables(env_variables);
    free(env_variables);
    for (size_t c = 0; c < i; c++)
        if (fds[c] != -1)
            close(fds[c]);

    close(save_stdin);
    close(save_stdout);
    free(fds);

    return out;
}
