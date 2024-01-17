#ifndef LIST_PARSER_H
#define LIST_PARSER_H

#include "parser/parser.h"

enum parser_status parse_list(struct ast **res, struct lexer *lexer);

enum parser_status parse_compound_list(struct ast **res, struct lexer *lexer);

#endif /* LIST_PARSER_H */
