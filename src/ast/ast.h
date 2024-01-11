#ifndef AST_H
#define AST_H

enum ast_type
{
    NODE_COMMAND,
    NODE_SEMICOLON,
    NODE_IF,

};

union ast_content
{
    struct ast *compound_list;
    char **command;
};

struct ast
{
    union ast_content content;
    enum ast_type node_type; // Type of node

    struct ast *right_child; // Right child, NULL if it dosn't exist
    struct ast *left_child; // Left child, NULL if it donsn't exist
};


int execute_command(struct ast *ast);
int handle_if(struct ast *ast);
int handle_semicolon(struct ast *ast);
int evaluate_ast(struct ast *ast);

#endif /* !AST_H */
