noinst_LIBRARIES = liblexer.a

liblexer_a_SOURCES = lexer.c lexer.h\
		     token.h\
		     handle_special_cases.c handle_special_cases.h\
		     get_token_type.c get_token_type.h

liblexer_a_CPPFLAGS = -I../ -D_POSIX_C_SOURCE=200809L
liblexer_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic
