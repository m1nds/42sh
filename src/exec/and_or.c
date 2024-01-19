#include <stddef.h>

#include "ast/ast.h"
#include "builtin/builtin.h"
#include "exec.h"

int handle_and(struct ast *ast)
{
    if (ast->node_type != NODE_AND)
    {
        return -1;
    }
    size_t i = 0;
    while (ast->children[i])
    {
        int val = evaluate_ast(ast->children[i]);
        if (val == false_builtin())
        {
            return false_builtin();
        }
        i++;
    }
    return true_builtin();
}

int handle_or(struct ast *ast)
{
    if (ast->node_type != NODE_OR)
    {
        return -1;
    }

    size_t i = 0;
    while (ast->children[i])
    {
        int val = evaluate_ast(ast->children[i]);
        if (val == true_builtin())
        {
            return true_builtin();
        }
        i++;
    }
    return false_builtin();
}
