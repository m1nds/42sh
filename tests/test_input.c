#include <criterion/criterion.h>
#include <stdio.h>

#include "input/input.h"

Test(input, on_stdin)
{
    int flag = 0;
    char *argv[] = { "42sh", NULL };
    FILE *fd = parse_input(1, argv, &flag);
    cr_expect_eq(fd, stdin);
}

Test(input, on_string)
{
    int flag = 0;
    char *argv[] = { "42sh", "-c", "echo hello world", NULL };
    FILE *fd = parse_input(3, argv, &flag);
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
    int flag = 0;
    char *str = "echo hello world\n";
    char *argv[] = { "42sh", "test_input_file", NULL };
    FILE *fd = parse_input(2, argv, &flag);
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

Test(input, check_flags)
{
    int flags = 0;
    char *argv[] = { "42sh", "--verbose", "--pretty-print", NULL };
    FILE *fd = parse_input(3, argv, &flags);
    cr_expect_eq(flags, 0b11);
    fclose(fd);
}
