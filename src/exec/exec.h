#ifndef EXEC_H
#define EXEC_H

#include <stddef.h>

#include "ast/ast.h"

/*
 * @def:   Check if command is a builtin and runs it if it is one
 * @param: char **value NULL-terminated array of strings, representing a command
 * @ret:   return code of the builtin or -1 if it is not a builtin
 * */
int check_builtin(char **value);

/*
 * @def:   Evaluation handler for pipelines.
 * @param: struct ast *ast: ast node containing a pipeline in the value field.
 * @ret:   exit code of the executed command or -1 on error.
 * */
int handle_pipe(struct ast *ast);

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
 * @def: Evaluation handler for negation.
 * @param: struct ast *ast: AST node containing commands in the children field.
 * @ret: return code of the negation on the correct condition node or -1 on
 * error.
 * */
int handle_negation(struct ast *ast);

/*
 * @def: Evaluation handler for and.
 * @param: struct ast *ast: AST node containing commands in the children field.
 * @ret: return code
 * */
int handle_and(struct ast *ast);

/*
 * @def: Evaluation handler for or.
 * @param: struct ast *ast: AST node containing commands in the children field.
 * @ret: return code
 * */
int handle_or(struct ast *ast);

/*
 * @def:   Complete AST evaluation function
 * @param: struct ast *ast: the complete AST
 * @ret:   exit code or -1 if an error occurs
 * */
int evaluate_ast(struct ast *ast);

#endif /* ! EXEC_H */
