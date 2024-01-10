SUBDIRS = ast exec input lexer parser builtin

bin_PROGRAMS = 42sh

42sh_SOURCES = main.c

42sh_CPPFLAGS = -I%D%
42sh_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

42sh_LDADD = ast/libast.a \
		exec/libexec.a \
		input/libinput.a \
		lexer/liblexer.a \
		parser/libparser.a \
		builtin/libbuiltin.a
