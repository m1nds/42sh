#ifndef AST_H
#define AST_H

#include <stddef.h>

enum ast_type
{
    NODE_COMMAND,
    NODE_SEMICOLON,
    NODE_IF

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
 * @def:   Evaluation handler for commands. Checks if it is a builtin.
 * @param: struct ast *ast: ast node containing a command in the value field.
 * @ret:   exit code of the executed command or -1 on error.
 * */
int handle_command(struct ast *ast);

/*
 * @def:   Evaluation handler for if conditions.
 * @param: struct ast *ast: ast node containing children corresponding to a
 * condition. (children field)
 * @ret:   return code of the evaluation on the correct condition node or -1 on
 * error.
 * */
int handle_if(struct ast *ast);

/*
 * @def:   Evaluation handler for semi-colon.
 * @param: struct ast *ast: AST node containing commands in the children field.
 * @ret:   return code of the last command node or -1 on error.
 * */
int handle_semicolon(struct ast *ast);

/*
 * @def:   Complete AST evaluation function
 * @param: struct ast *ast: the complete AST
 * @ret:   exit code or -1 if an error occurs
 * */
int evaluate_ast(struct ast *ast);

#endif /* ! AST_H */
