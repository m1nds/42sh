#include <criterion/criterion.h>
#include <stdio.h>

#include "input/input.h"

Test(input, on_stdin)
{
    int err = 0;
    int flag = 0;
    char *argv[] = { "42sh", NULL };
    FILE *fd = get_input(1, argv, &flag, &err);
    cr_expect_eq(fd, stdin);
}

Test(input, on_string)
{
    int err = 0;
    int flag = 0;
    char *argv[] = { "42sh", "-c", "echo hello world", NULL };
    FILE *fd = get_input(3, argv, &flag, &err);
    size_t i = 0;
    char c = fgetc(fd);
    while (c != EOF)
    {
        cr_assert_eq(c, argv[2][i]);
        c = fgetc(fd);
        i++;
    }
    fclose(fd);
}

Test(input, on_file)
{
    int err = 0;
    int flag = 0;
    char *str = "echo hello world\n";
    char *argv[] = { "42sh", "test_input_file", NULL };
    FILE *fd = get_input(2, argv, &flag, &err);
    size_t i = 0;
    char c = fgetc(fd);
    while (c != EOF)
    {
        cr_assert_eq(c, str[i]);
        c = fgetc(fd);
        i++;
    }
    fclose(fd);
}

Test(input_flags, check_flags1)
{
    int err = 0;
    int flags = 0;
    char *argv[] = { "42sh", "--verbose", NULL };
    FILE *fd = get_input(2, argv, &flags, &err);
    cr_expect_eq(flags, 0b01);
    fclose(fd);
}

Test(input_flags, check_flags2)
{
    int err = 0;
    int flags = 0;
    char *argv[] = { "42sh", "--pretty-print", NULL };
    FILE *fd = get_input(2, argv, &flags, &err);
    cr_expect_eq(flags, 0b10);
    fclose(fd);
}

Test(input_flags, check_flags3)
{
    int err = 0;
    int flags = 0;
    char *argv[] = { "42sh", "--verbose", "--pretty-print", NULL };
    FILE *fd = get_input(3, argv, &flags, &err);
    cr_expect_eq(flags, 0b11);
    fclose(fd);
}

Test(check_flags, chck_verbose)
{
    int err = 0;
    int flags = 0;
    char *argv[] = { "42sh", "--verbose", NULL };
    FILE *fd = get_input(2, argv, &flags, &err);
    int res = check_verbose(&flags);
    cr_expect_eq(res, VERBOSE_FLAG);
    fclose(fd);
}

Test(check_flags, chck_prettyprint)
{
    int err = 0;
    int flags = 0;
    char *argv[] = { "42sh", "--pretty-print", NULL };
    FILE *fd = get_input(2, argv, &flags, &err);
    int res = check_prettyprint(&flags);
    cr_expect_eq(res, PRETTY_PRINT_FLAG);
    fclose(fd);
}

Test(check_flags, chck_verb_both)
{
    int err = 0;
    int flags = 0;
    char *argv[] = { "42sh", "--verbose", "--pretty-print", NULL };
    FILE *fd = get_input(2, argv, &flags, &err);
    int res = check_verbose(&flags);
    cr_expect_eq(res, VERBOSE_FLAG);
    fclose(fd);
}
