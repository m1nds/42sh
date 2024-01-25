#ifndef PREPROCESSING_H
#define PREPROCESSING_H

#include "ast/ast.h"

void setup_value(char *key, char *value);

void set_exit_value(int value);

void replace_variables(struct ast *ast);

#endif /* PREPROCESSING_H */
