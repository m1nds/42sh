#ifndef AST_H
#define AST_H

#include <stddef.h>

enum ast_type
{
    NODE_COMMAND,
    NODE_SEMICOLON,
    NODE_IF,
    NODE_PIPE,
    NODE_NOT,
    NODE_AND,
    NODE_OR,
    NODE_WHILE,
    NODE_UNTIL,
    NODE_REDIR,
    NODE_REDIR_IN,
    NODE_REDIR_OUT,
    NODE_REDIR_OUTA,
    NODE_REDIR_INAND,
    NODE_REDIR_OUTAND,
    NODE_REDIR_INOUT

};

struct ast
{
    enum ast_type node_type; // Type of node
    char **value; // The commande and it's argument the node contain NULL T
    struct ast **children; // list of the children of the node NULL T
};

/*
 * @def:   Create a new ast node and return it
 * @param: enum ast_type type: the type of node you want to create
 *         size_t nb_child: the number of child it should have
 *         char *value: the value of the first child set at ast->children[0]
 * @ret:   return the allocated ast struct or NULL if malloc failled
 * */
struct ast *ast_new(enum ast_type type, size_t nb_child, char *value);

/*
 * @def:   free the struct ast, all its children recursively and their **value
 * @param: struct ast *ast: the ast node that should be freed
 * @ret:   None
 * */
void ast_free(struct ast *ast);

/*
 * @def:   print on stdout the ast recursively on its children
 * @param: struct ast *ast: the ast node that should be printed
 * @ret:   None
 * */
void print_ast(struct ast *ast);

#endif /* ! AST_H */
