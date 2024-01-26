#ifndef BUILTIN_H
#define BUILTIN_H

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

#endif /* !BUILTIN_H */
