#include "ast/ast.h"
#include "builtin/builtin.h"
#include "exec.h"

int handle_if(struct ast *ast)
{
    if (ast->node_type != NODE_IF)
    {
        return -1;
    }

    int condition = evaluate_ast(ast->children[0]);
    if (condition == true_builtin())
    {
        return evaluate_ast(ast->children[1]);
    }

    return (ast->children[2] != NULL) ? evaluate_ast(ast->children[2]) : 0;
}
