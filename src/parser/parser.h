#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"

enum parser_status
{
    PARSER_OK,
    PARSER_UNEXPECTED_TOKEN,
};

/**
 * @def: Parses the input and builds the ast
 *
 * input =    list '\n'
 *          | list EOF
 *          | '\n'
 *          | EOF
 *          ;
 */
enum parser_status parse_input(struct ast **res, struct lexer *lexer);

/**
 * @def: Parses a list of and_ors
 *
 * list =       and_or ;
 */
enum parser_status parse_list(struct ast **res, struct lexer *lexer);

/**
 * @def: Parses multiple pipelines seperated by ands and ors
 *
 * and_or =     pipeline ;
 */
enum parser_status parse_and_or(struct ast **res, struct lexer *lexer);

/**
 * @def: Parses multiple commands seperated by pipelines
 *
 * pipeline =   command ;
 */
enum parser_status parse_pipeline(struct ast **res, struct lexer *lexer);

/**
 * @def: Parses a command comprised of multiple simple commands
 *
 * command =    simple_command ;
 */
enum parser_status parse_command(struct ast **res, struct lexer *lexer);

/**
 * @def: Parses a simple_command comprised of words and elements
 *
 * simple_command =     WORD { element } ;
 */
enum parser_status parse_simple_command(struct ast **res, struct lexer *lexer);

/**
 * @def: Parses a element comprised of a WORD
 *
 * element =     WORD ;
 */
enum parser_status parse_element(struct ast **res, struct lexer *lexer);

#endif /* !PARSER_H */
