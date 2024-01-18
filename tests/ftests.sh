#!/bin/sh

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

pass=0
fail=0

test_stdin() {
    echo "$1" | ./42sh 1> output1 2> /dev/null
    echo "$1" | bash --posix 1> output2 2> /dev/null

    flag=0

    out1=$(cat -e output1)
    out2=$(cat -e output2)

    echo "$1" | ./42sh > /dev/null 2> /dev/null
    code1=$?
    echo "$1" | bash --posix > /dev/null 2> /dev/null
    code2=$?

    diff output1 output2 > /dev/null
    check=$?

    if [ $check -eq 1 ] && [ "$code1" -ne "$code2" ]; then
        echo -e "  ${RED}ERROR${NC} IN TEST : \"$2\"\n  Type: STDIN\n  Print: ${GREEN}expected :${NC}\n$out2\n  ${RED}got :${NC}\n$out1"
        echo -e "  Return Code: ${GREEN}expected :${NC}  $code2  ${RED}got :${NC}  $code1\n"
        return 1
    fi

    return 0
}

test_file() {
    echo "$1" > file

    ./42sh file > output1 2> /dev/null
    bash --posix file > output2 2> /dev/null

    flag=0

    out1=$(cat -e output1)
    out2=$(cat -e output2)

    ./42sh file > /dev/null 2> /dev/null
    code1=$?
    bash --posix file > /dev/null 2> /dev/null
    code2=$?

    diff output1 output2 > /dev/null
    check=$?

    if [ $check -eq 1 ] || [ "$code1" -ne "$code2" ]; then
        echo -e "  ${RED}ERROR${NC} IN TEST : \"$2\"\n  Type: FILE\n  Print: ${GREEN}expected :${NC}\n$out2\n  ${RED}got :${NC}\n$out1"
        echo -e "  Return Code: ${GREEN}expected :${NC}  $code2  ${RED}got :${NC}  $code1\n"
        return 1
    fi

    return 0
}

test_string() {
    ./42sh -c "$1" > output1 2> /dev/null
    bash --posix -c "$1" > output2 2> /dev/null

    flag=0

    out1=$(cat -e output1)
    out2=$(cat -e output2)

    ./42sh -c "$1" > /dev/null 2> /dev/null
    code1=$?
    bash --posix -c "$1" > /dev/null 2> /dev/null
    code2=$?

    diff output1 output2 > /dev/null
    check=$?
    if [ $check -eq 1 ] || [ "$code1" -ne "$code2" ]; then
        echo -e "  ${RED}ERROR${NC} IN TEST : \"$2\"\n  Type: STRING\n  Print: ${GREEN}expected :${NC}\n$out2\n  ${RED}got :${NC}\n$out1"
        echo -e "  Return Code: ${GREEN}expected :${NC}  $code2  ${RED}got :${NC}  $code1\n"
        return 1
    fi

    return 0
}

test_all() {
    echo -e "Running TEST : $2..."
    test_string "$1" "$2"
    one=$?
    test_file "$1" "$2"
    two=$?
    test_stdin "$1" "$2"
    three=$?

    if [ $(($one + $two + $three)) -eq 0 ]; then
        pass=$(($pass + 1))
    else
        fail=$(($fail + 1))
    fi
}

test_all "true;" "true;"
test_all "echo a ; echo b" "double_echo"
test_all "echo -E 'ab\n'" "echo_flag_E"
test_all "echo -e 'salam\n'" "echo_flag_e"
test_all "echo -n 'mon boeuf\n'" "echo_flag_n"
test_all "echo -nE 'jure mon boeuf\n'" "echo_flag_nE"
test_all "echo -Ee 't vraiment un sang\n'" "echo_flag_Ee"
#test_all "echo -e  \\\\q 
#       \n\\n    \\\\n            \t        " "echo some weird stuff"
test_all 'if true; then echo nice; fi' "if_echo"
test_all 'if false; then echo not printed; else echo printed; fi' "else_echo"
test_all 'if false; then true; elif true; then echo printed; fi' "elif_echo"
test_all 'if; then true; fi' "wrong_if"
test_all "if false; then echoa; else echo b; fi" "wrong command never executed"
test_all "if false; then echoa; elif echo b; fi; fi" "wrong fi after elif"
test_all "if false; then echoa; elif echo b; then fi; fi" "wrong fi after then"
test_all 'if if true; then echo a; else echo b; fi; then echo false; fi' "double_monstre"
test_all "echo a; echo #this shouldnt be echoed" "comment"
test_all "echo '#''quoted'" "quotes"
test_all "echo a
echo b" "echo \\\\n echo"
test_all "echo a;;" "extra ;"
test_all "if false; then true; elif false; then echo a b; elif true; then echo a; ls; fi;" "multiple elifs"
test_all "if true; then fi;" "wrong then"
test_all "ls" "not builtin"
test_all "false true
" "false with argument"
test_all "echo abnfsjfisfnsiofnsfbhsiofniafuisbfosagefiseofgbuasfgb" "long echo"
#test_all "echo a | cat -e" "easy pipe"
#test_all "ls | cat -e | cat -e" "normal pipe"
#test_all "tree | cat -e | grep 42sh" "hard pipe"
#test_all "echo ab c |" "error pipe"
#test_all 'for i in  "asasasasa" ; do echo $i ; done' "for"

echo -e "${GREEN}Passed: $pass ${NC}, ${RED}Failed $fail${NC}"
rm output1 output2 file 2> /dev/null > /dev/null


if [ "$fail" -eq 0 ]; then
    exit 0
fi

exit 1

