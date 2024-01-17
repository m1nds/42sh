#include "ast/ast.h"
#include "exec.h"

int handle_semicolon(struct ast *ast)
{
    if (ast->node_type != NODE_SEMICOLON)
    {
        return -1;
    }

    struct ast **current_child = ast->children;
    int out = 0;

    while (*current_child)
    {
        out = evaluate_ast(*current_child);
        current_child++;
    }

    return out;
}
