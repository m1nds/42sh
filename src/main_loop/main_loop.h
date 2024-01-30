#ifndef MAIN_LOOP_H
#define MAIN_LOOP_H

#include <stdio.h>

/*
 * @def:   Main execution loop of 42sh
 * @param: FILE* input: stream that leads to the code to be interpreted.
 * @param: int options: represents the options (--pretty-print, etc...)
 * @ret:   exit code
 * */
int main_execution_loop(FILE *input, int options);

#endif /* ! MAIN_LOOP_H */
