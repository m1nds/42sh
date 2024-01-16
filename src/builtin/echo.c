#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "builtin.h"

static void echo_int(char **list_str, size_t i, char flag)
{
    char c;
    while (list_str[i] != NULL)
    {
        size_t y = 0;
        while (list_str[i][y] != '\0')
        {
            if (flag && list_str[i][y] == '\\')
            {
                y++;
                switch (list_str[i][y])
                {
                case '\\':
                    c = '\\';
                    break;
                case 't':
                    c = '\t';
                    break;
                case 'n':
                    c = '\n';
                    break;
                default:
                    y--;
                    c = list_str[i][y];
                    break;
                }
            }
            else
                c = list_str[i][y];
            putchar(c);
            y++;
        }
        i++;
        if (list_str[i] != NULL)
        {
            putchar(' ');
        }
    }
}

int echo_builtin(char **list_str)
{
    if (list_str == NULL)
        return 500000;
    char flag_n = 1; // if set to 1 \n is add at the end of the string
    char flag_special_char = 0; // if set to 1 special char are replace
    char break_flag = 0;
    size_t i = 1;
    while (list_str[i] != NULL)
    {
        size_t j = 0;
        char flag_option = 0;
        while (list_str[i][j] != '\0')
        {
            if (flag_option && list_str[i][j] == 'n')
            {
                flag_n = 0;
            }
            else if (flag_option && list_str[i][j] == 'e')
            {
                flag_special_char = 1;
            }
            else if (flag_option && list_str[i][j] == 'E')
            {
                flag_special_char = 0;
            }
            else if (list_str[i][j] == '-')
            {
                if (flag_option || list_str[i][j + 1] == '\0')
                {
                    break_flag = 1;
                    break;
                }
                flag_option = 1;
            }
            else
            {
                break_flag = 1;
                break;
            }
            j++;
        }
        if (break_flag)
        {
            break;
        }
        i++;
    }
    echo_int(list_str, i, flag_special_char);
    if (flag_n)
    {
        putchar('\n');
    }
    fflush(stdout);
    return 0;
}
