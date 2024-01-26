noinst_LIBRARIES = libexec.a

libexec_a_SOURCES = exec.h \
					exec.c \
					command.c \
					if.c \
					semicolon.c \
					pipeline.c \
					negation.c \
					and_or.c \
					check_builtin.c \
					loop.c \
					redirections.c \
                                        function.c

libexec_a_CPPFLAGS = -I../ -D_POSIX_C_SOURCE=200809L
libexec_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic
