#ifndef PARSER_H
#define PARSER_H

#include "ast/ast.h"
#include "lexer/lexer.h"

enum parser_status
{
    PARSER_OK,
    PARSER_UNEXPECTED_TOKEN,
    PARSER_REALLY_NOT_OK,
};
/*
 * @def: prints an error
 */
void print_error(char *message, struct lexer *lexer);

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
 * list =  and_or { ';' and_or } [ ';' ] ;
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
 * @def: Parses a command comprised of multiple simple commands or a shell
 * command
 *
 * command =      simple_command
 *              | shell_command
 *              ;
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

/**
 * @def: Parses a element comprised of a rule if, while and until
 *
 * shell_command = rule_if
 *               | rule_while
 *               | rule_until
 *               | rule_for
 *               ;
 */
enum parser_status parse_shell_command(struct ast **res, struct lexer *lexer);

/**
 * @def: Parses a element comprised of an if then else
 *
 * rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi' ;
 */
enum parser_status parse_rule_if(struct ast **res, struct lexer *lexer);

/**
 * @def: Parses a element comprised of an else clause
 *
 * else_clause  =  'else' compound_list
 *               | 'elif' compound_list 'then' compound_list [else_clause]
 *               ;
 */
enum parser_status parse_else_clause(struct ast **res, struct lexer *lexer);

/**
 * @def: Parses a element comprised of a rule until
 *
 * while_rule  =   'while' compound_list 'do' compound_list 'done' ;
 * */
enum parser_status parse_rule_while(struct ast **res, struct lexer *lexer);

/**
 * @def: Parses a element comprised of a rule until
 *
 * until_rule  =   'until' compound_list 'do' compound_list 'done' ;
 */
enum parser_status parse_rule_until(struct ast **res, struct lexer *lexer);

/**
 * @def: Parses a element comprised of a rule for
 *
 * for_rule  =   'for' WORD ( [';'] | [{'\n'} 'in' { WORD } (';' | '\n')])
 *               {'\n'} 'do' compound_list 'done';
 */
enum parser_status parse_rule_for(struct ast **res, struct lexer *lexer);

/**
 * @def: Parses a element comprised of a compound_list
 *
 * compound_list = {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'} ;
 */
enum parser_status parse_compound_list(struct ast **res, struct lexer *lexer);

/*
 * @def: Parses a prefix that can be before a command
 *
 * prefix =
 *  ASSIGNMENT_WORD
 *  | redirection
 *  ;
 */
enum parser_status parse_prefix(struct ast **res, struct lexer *lexer);

/*
 * @def: Parses a single redirection that can be preceded by a io_number
 *
 * redirection = [IONUMBER] ( '>' | '<' | '>>' | '>&' | '<&' | '>|' | '<>' )
 *               WORD ;
 */
enum parser_status parse_redirection(struct ast **res, struct lexer *lexer);

/*
 * @def: Parses a function comprised of its name and definition
 *
 * funcdec = WORD '(' ')' {'\n'} shell_command ;
 */
enum parser_status parse_funcdec(struct ast **res, struct lexer *lexer);

#endif /* !PARSER_H */
