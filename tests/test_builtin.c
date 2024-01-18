#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <stdio.h>

#include "builtin/builtin.h"

Test(true_builtin, default_behavior)
{
    cr_expect_eq(true_builtin(), 0, "Expected: %d. Got: %d.", 0,
                 true_builtin());
}

Test(false_builtin, default_behavior)
{
    cr_expect_eq(false_builtin(), 1, "Expected: %d. Got: %d.", 1,
                 false_builtin());
}

Test(echo_builtin, hello_world)
{
    cr_redirect_stdout();
    char *str[] = { "echo", "Hello World!", NULL };
    echo_builtin(str);
    fflush(stdout);
    cr_assert_stdout_eq_str("Hello World!\n");
}

Test(echo_builtin, empty)
{
    cr_redirect_stdout();
    char *str[] = { "echo", NULL };
    echo_builtin(str);
    fflush(stdout);
    cr_assert_stdout_eq_str("\n");
}

Test(echo_builtin, empty_n)
{
    cr_redirect_stdout();
    char *str[] = { "echo", "-n", NULL };
    echo_builtin(str);
    fflush(stdout);
    cr_assert_stdout_eq_str("");
}

Test(echo_builtin, backslash_n_flag_e)
{
    cr_redirect_stdout();
    char *str[] = { "echo", "-e", "\\n", NULL };
    echo_builtin(str);
    fflush(stdout);
    cr_assert_stdout_eq_str("\n\n");
}

Test(echo_builtin, backslash_n_flag_E)
{
    cr_redirect_stdout();
    char *str[] = { "echo", "-E", "\\n", NULL };
    echo_builtin(str);
    fflush(stdout);
    cr_assert_stdout_eq_str("\\n\n");
}

Test(echo_builtin, multi_flag)
{
    cr_redirect_stdout();
    char *str[] = { "echo", "-E", "-n", "\\n", NULL };
    echo_builtin(str);
    fflush(stdout);
    cr_assert_stdout_eq_str("\\n");
}

Test(echo_builtin, redefinition_of_flag)
{
    cr_redirect_stdout();
    char *str[] = { "echo", "-E", "-e", "\\n", NULL };
    echo_builtin(str);
    fflush(stdout);
    cr_assert_stdout_eq_str("\n\n");
}

Test(echo_builtin, multi_flags_together1)
{
    cr_redirect_stdout();
    char *str[] = { "echo", "-en", "\\n", NULL };
    echo_builtin(str);
    fflush(stdout);
    cr_assert_stdout_eq_str("\n");
}

Test(echo_builtin, multi_flags_together2)
{
    cr_redirect_stdout();
    char *str[] = { "echo", "-eEn", "\\n", NULL };
    echo_builtin(str);
    fflush(stdout);
    cr_assert_stdout_eq_str("\\n");
}

Test(echo_builtin, multi_flags_together3)
{
    cr_redirect_stdout();
    char *str[] = { "echo", "-nnE", "\\n", NULL };
    echo_builtin(str);
    fflush(stdout);
    cr_assert_stdout_eq_str("\\n");
}
