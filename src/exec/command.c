#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast/ast.h"
#include "builtin/builtin.h"
#include "exec/exec.h"
#include "variables/preprocessing.h"

/*int check_builtin(char **value)
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
}*/

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
    replace_variables(ast);
    int cb = check_builtin(ast->value);
    if (cb >= 999)
    {
        return cb;
    }
    if (cb != -1)
    {
        // Set $? to cb
        set_exit_value(cb);
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
            errx(127, "Could not find command %s", ast->value[0]);
        }
    }

    int status;
    waitpid(pid, &status, 0);
    // Set $? to status
    set_exit_value(WEXITSTATUS(status));
    return WEXITSTATUS(status);
}
