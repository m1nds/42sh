noinst_LIBRARIES = libparser.a

libparser_a_SOURCES = parser.h parser.c\
		      list_parser.h list_parser.c\
		      shell_parser.h shell_parser.c\
		      redirect_parser.c

libparser_a_CPPFLAGS = -I../ -D_POSIX_C_SOURCE=200809L
libparser_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic
