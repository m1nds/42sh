#include <stdlib.h>
#include <sys/wait.h>

#include "ast/ast.h"
#include "exec/exec.h"
#include "utils/hash_map.h"
#include "variables/variables.h"

int real_exit_code(int out)
{
    if (out >= 999)
    {
        if (out == 999)
        {
            struct hash_map *hm_vars = get_variables();
            char *ret = hash_map_get(hm_vars, "?");
            if (ret == NULL)
            {
                out = 0;
            }
            else
            {
                out = atoi(ret);
            }
        }
        else
        {
            out -= 1000;
        }
    }
    return out;
}

int handle_subshell(struct ast *ast)
{
    if (ast == NULL)
    {
        return -1;
    }
    // Save both variable and function hash_map
    struct hash_map *hm_vars = hash_map_copy(get_variables());
    struct hash_map *hm_funcs = hash_map_copy(get_functions());
    // Execute the subshell in child
    int fd = fork();
    if (fd == 0)
    {
        int out = evaluate_ast(ast->children[0]);
        exit(real_exit_code(out));
    }
    int wstatus;
    waitpid(fd, &wstatus, 0);
    int out = WEXITSTATUS(wstatus);
    out = real_exit_code(out);
    // Restore variable and function hash_map
    hash_map_free(get_variables());
    hash_map_free(get_functions());
    set_variables(hm_vars);
    set_functions(hm_funcs);
    set_exit_value(out);
    return out;
}
