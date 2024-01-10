check_PROGRAMS = test

test_SOURCES = test_ast.c test_lexer.c

test_LDADD = \
		-lcriterion \
		%D%/../src/ast/libast.a \
		%D%/../src/input/libinput.a \
		%D%/../src/exec/libexec.a \
		%D%/../src/lexer/liblexer.a \
		%D%/../src/parser/libparser.a

check-local: test
	mv %D%/../src/42sh .
	./test
