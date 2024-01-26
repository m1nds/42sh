#include "ast/ast.h"
#include "builtin/builtin.h"
#include "exec.h"

int handle_negation(struct ast *ast)
{
    if (ast->node_type != NODE_NOT)
    {
        return -1;
    }

    int val = evaluate_ast(ast->children[0]);
    if (val >= 999)
    {
        return val;
    }
    if (val == true_builtin())
    {
        return false_builtin();
    }

    if (val == false_builtin())
    {
        return true_builtin();
    }

    return -1;
}
