check_PROGRAMS = test

test_SOURCES = test_parser.c test_ast.c test_lexer.c test_builtin.c test_input.c
test_CPPFLAGS = -I../src/ -D_POSIX_C_SOURCE=200809L

test_LDADD = \
		-lcriterion \
		%D%/../src/exec/libexec.a \
		%D%/../src/builtin/libbuiltin.a \
		%D%/../src/main_loop/libmainloop.a \
		%D%/../src/input/libinput.a \
		%D%/../src/lexer/liblexer.a \
		%D%/../src/parser/libparser.a \
		%D%/../src/ast/libast.a \
		%D%/../src/variables/libvariables.a \
		%D%/../src/utils/libutils.a

check-local: test
	cp %D%/../src/42sh .
	./test
	./ftests.sh
