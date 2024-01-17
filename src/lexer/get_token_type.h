#ifndef GET_TOKEN_TYPE_H
#define GET_TOKEN_TYPE_H

#include "lexer.h"

struct lexer_token_save match_word(char *word);

struct lexer_token_save get_special_character(struct lexer *lexer, char c);

#endif /* GET_TOKEN_TYPE_H */
