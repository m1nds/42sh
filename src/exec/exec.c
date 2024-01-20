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
    case NODE_NOT:
        return handle_negation(ast);
    case NODE_AND:
        return handle_and(ast);
    case NODE_OR:
        return handle_or(ast);
    case NODE_PIPE:
        return handle_pipe(ast);
    case NODE_WHILE:
        return handle_while(ast);
    case NODE_UNTIL:
        return handle_until(ast);
    case NODE_REDIR:
        return handle_redirect(ast);
    default:
        return -1;
    }
}
