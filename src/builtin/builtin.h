#ifndef BUILTIN_H
#define BUILTIN_H

#include "utils/vector.h"

static inline int true_builtin(void)
{
    return 0;
}

static inline int false_builtin(void)
{
    return 1;
}

int echo_builtin(char **list_str);

int unset(char **list_str);

int dot(char **list_str);

int export(char **list_str);

int exit_builtin(char **list_str);

/*
 * @def: Change a path in its canonical form (handle double-dots,
 *       multiple slashes, single dots and does not follow symlinks)
 * @args vector: the vector where the path is stored
 * @return : a vector containing the
 *
 * NB : You have to use vector_destroy() to free both of the vectors.
 */
struct vector *canonical_path(struct vector *path);

/*
 * @def: cd builtin handler. Changes $OLDPWD, $PWD and uses chdir()
 *       to update the current working directory.
 * @args args: buffer of strings that represents the arguments
 * @return : exit code of cd
 *
 * NB : You have to use vector_destroy() to free both of the vectors.
 */
int cd_builtin(char **args);

#endif /* !BUILTIN_H */
