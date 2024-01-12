#ifndef AST_H
#define AST_H

#include <stddef.h>

enum ast_type
{
    NODE_COMMAND,
    NODE_SEMICOLON,
    NODE_IF,

};

struct ast
{
    enum ast_type node_type; // Type of node
    char **value; // The commande and it's argument the node contain NULL T
    struct ast **children; // list of the children of the node NULL T
};

struct ast *ast_new(enum ast_type type, size_t nb_child, char *value);

void ast_free(struct ast *ast);

/*
int execute_command(struct ast *ast);
int handle_if(struct ast *ast);
int handle_semicolon(struct ast *ast);
int evaluate_ast(struct ast *ast);
*/

#endif /* !AST_H */
