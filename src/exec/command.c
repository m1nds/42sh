#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast/ast.h"
#include "builtin/builtin.h"
#include "exec/exec.h"
#include "variables/preprocessing.h"
#include "variables/variables.h"

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

int handle_command(struct ast *ast, bool replace)
{
    if (ast->node_type != NODE_COMMAND)
    {
        return -1;
    }
    if (ast->value == NULL)
    {
        return 0;
    }
    if (replace == true)
    {
        replace_variables(ast);
    }
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

    struct hash_map *hm_funcs = get_functions();
    struct ast *ast_fun = hash_map_get(hm_funcs, ast->value[0]);

    if (ast_fun != NULL)
    {
        cb = evaluate_ast(ast_fun);
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
