#ifndef HANDLE_SPECIAL_CASES
#define HANDLE_SPECIAL_CASES

#include "lexer.h"

/*
 * @def: Skips blanks in the lexer and returns the next character
 * @args c: the first character
 */
char skip_blanks(struct lexer *lexer, char c);

/*
 * @def: Handles the case where the word starts with a \
 */
struct lexer_token_save handle_escape(struct lexer *lexer, char c);

/*
 * @def: Handles the case where the word starts with a single quote
 */
struct lexer_token_save handle_single_quote(struct lexer *lexer);

#endif /* HANDLE_SPECIAL_CASES */
