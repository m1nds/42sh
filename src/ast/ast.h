#ifndef AST_H
#define AST_H

#include "token.h"

struct ast
{
    enum token node_type; // Type of node
    char **value; // NULL terminated array of char *
    struct ast *right_child; // Right child, NULL if it dosn't exist
    struct ast *left_child; // Left child, NULL if it donsn't exist
};

#endif /* !AST_H */
