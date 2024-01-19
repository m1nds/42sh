#include <stdio.h>
#include <string.h>

#include "ast/ast.h"
#include "exec/exec.h"

int handle_while(struct ast *ast)
{
    if (ast->node_type != NODE_WHILE)
    {
        return -1;
    }

    int condition = evaluate_ast(ast->children[0]);
    int retour = 0;
    while (condition == 0)
    {
        retour = evaluate_ast(ast->children[1]);
        condition = evaluate_ast(ast->children[0]);
    }

    return retour;
}

int handle_until(struct ast *ast)
{
    if (ast->node_type != NODE_UNTIL)
    {
        return -1;
    }

    int condition = evaluate_ast(ast->children[0]);
    int retour = 0;
    while (condition != 0)
    {
        retour = evaluate_ast(ast->children[1]);
        condition = evaluate_ast(ast->children[0]);
    }

    return retour;
}
