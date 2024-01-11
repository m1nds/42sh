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
    echo_builtin("Hello World!");
    fflush(stdout);
    cr_assert_stdout_eq_str("Hello World!\n");
}
