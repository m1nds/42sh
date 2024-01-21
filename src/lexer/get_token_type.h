#ifndef GET_TOKEN_TYPE_H
#define GET_TOKEN_TYPE_H

#include "lexer/lexer.h"

/*
 * @def: Matches the word and returns the appropriate token
 *       Also checks if the word is a io_number
 */
struct lexer_token_save match_word(struct lexer *lexer, char *word);

/*
 * @def: Handles the case of most special characters
 * (\, <, >, ;, EOF, ...)
 */
struct lexer_token_save get_special_character(struct lexer *lexer, char c);

#endif /* GET_TOKEN_TYPE_H */
