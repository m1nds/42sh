#ifndef HANDLE_SPECIAL_CASES_H
#define HANDLE_SPECIAL_CASES_H

#include "lexer.h"
#include "utils/vector.h"

/*
 * @def: Skips blanks in the lexer and returns the next character
 * @args c: the first character
 */
char skip_blanks(struct lexer *lexer, char c);

/*
 * @def: Handles the case where the word starts with a \
 */
struct lexer_token_save handle_escape(struct lexer *lexer);

/*
 * @def: Handles the case where the word contains with a single quote
 */
struct lexer_token_save handle_single_quote(struct lexer *lexer, char c);

/*
 * @def: Handles the case where the word contains a double quote
 */
struct lexer_token_save handle_double_quote(struct lexer *lexer, struct vector *vec);

#endif /* HANDLE_SPECIAL_CASES_H */
