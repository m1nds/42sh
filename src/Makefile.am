SUBDIRS = utils ast exec input lexer parser builtin variables main_loop

bin_PROGRAMS = 42sh

42sh_SOURCES = main.c

42sh_CPPFLAGS = -I%D%
42sh_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

42sh_LDADD =\
		main_loop/libmainloop.a \
		input/libinput.a \
		lexer/liblexer.a \
		parser/libparser.a \
		exec/libexec.a \
		ast/libast.a \
		builtin/libbuiltin.a\
		variables/libvariables.a\
		utils/libutils.a
