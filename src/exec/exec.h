#ifndef EXEC_H
#define EXEC_H

#include <stdbool.h>
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
 *         bool replace: determines if the command should have its
 *                       variables replaced
 * @ret:   exit code of the executed command or -1 on error.
 * */
int handle_command(struct ast *ast, bool replace);

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
 * @def:   Evaluation handler for while loop.
 * @param: struct ast *ast: AST node containing in Child[0] the compound_list
 *                          repressenting the condition
 *         in child[1] the compound_list to evaluate
 * @ret:   return code of the last command node or -1 on error.
 * */
int handle_while(struct ast *ast);

/*
 * @def:   Evaluation handler for until loop.
 * @param: struct ast *ast: AST node containing in Child[0] the compound_list
 *                          repressenting the condition
 *         in child[1] the compound_list to evaluate
 * @ret:   return code of the last command node or -1 on error.
 * */
int handle_until(struct ast *ast);

/*
 * @def:   Evaluation handler for for loop.
 * @param: struct ast *ast: AST node containing in Child[0] the compound_list
 *         in value[0] the variables that need to be assign at each loop
 *         in value[1_n-1] the different value it need to take
 * @ret:   return code of the last command node or -1 on error.
 * */
int handle_for(struct ast *ast);

/*
 * @def:   Evaluation handler for redirections.
 * @param: struct ast *ast: AST node containing in its children
 *         something to evaluate and one or many redirections
 * @ret:   return code of the last command node or -1 on error.
 * */
int handle_redirect(struct ast *ast);

/*
 * @def: Insert a new function in the function's hash map
 * @param: struct ast *ast: AST node containing the function
 * @ret: nothing
 * */
void insert_function(struct ast *ast);

/*
 * @def:   Complete AST evaluation function
 * @param: struct ast *ast: the complete AST
 * @ret:   exit code or -1 if an error occurs
 * */
int evaluate_ast(struct ast *ast);

#endif /* ! EXEC_H */
