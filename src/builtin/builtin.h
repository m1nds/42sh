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


int cd_builtin(char **args);

#endif /* !BUILTIN_H */
