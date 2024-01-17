#ifndef SHELL_PARSER_H
#define SHELL_PARSER_H

#include "parser/parser.h"

enum parser_status parse_shell_command(struct ast **res, struct lexer *lexer);

enum parser_status parse_rule_if(struct ast **res, struct lexer *lexer);

enum parser_status parse_else_clause(struct ast **res, struct lexer *lexer);

#endif /* SHELL_PARSER_H */
