noinst_LIBRARIES = libbuiltin.a

libbuiltin_a_SOURCES = builtin.h echo.c unset.c dot.c export.c exit.c

libbuiltin_a_CPPFLAGS = -I../ -D_POSIX_C_SOURCE=200809L
libbuiltin_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic
