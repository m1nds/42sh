AC_INIT([42sh], [1.0], [lyes.bourennani@epita.fr])

AM_INIT_AUTOMAKE([subdir-objects] [foreign])
AM_SILENT_RULES([yes])

#Checking for required Libs and Programs
AM_PROG_AR
AC_PROG_RANLIB
AC_PROG_CC


AC_CONFIG_FILES([
                    Makefile
                    src/Makefile
                    src/utils/Makefile
                    src/ast/Makefile
                    src/exec/Makefile
                    src/input/Makefile
                    src/lexer/Makefile
                    src/parser/Makefile
                    src/builtin/Makefile
                    src/variables/Makefile
                    tests/Makefile
                ])
AC_OUTPUT
