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

int echo_builtin(const char **list_str);

#endif /* !BUILTIN_H */
