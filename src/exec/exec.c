#include "exec.h"

#include "ast/ast.h"

int evaluate_ast(struct ast *ast)
{
    if (ast == NULL)
    {
        return 0;
    }

    switch (ast->node_type)
    {
    case NODE_COMMAND:
        return handle_command(ast);
    case NODE_SEMICOLON:
        return handle_semicolon(ast);
    case NODE_IF:
        return handle_if(ast);
    case NODE_PIPE:
        return handle_pipe(ast);
    default:
        return -1;
    }
}
