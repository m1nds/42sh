#!/bin/sh

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

pass=0
fail=0

test_stdin() {
    echo "$1" | ./42sh 1> output1 2> /dev/null
    code1=$?
    echo "$1" | bash --posix 1> output2 2> /dev/null
    code2=$?

    flag=0
    diff output1 output2 > /dev/null

    if [ $? -ne 0 ] && [ "$code1" -ne "$code2" ]; then
        flag=1
        out1=$(cat -e output1)
        out2=$(cat -e output2)
        echo -e "  ${RED}ERROR${NC} IN TEST : \"$2\"\n  Type: STDIN\n  Print: ${GREEN}expected :${NC}\n$out2\n  ${RED}got :${NC}\n$out1"
        echo -e "  Return Code: ${GREEN}expected :${NC}  $code2  ${RED}got :${NC}  $code1\n"
    fi

    if [ $# -ne 2 ]; then
        arg="$1"
        testname="$2"
        shift 2
        while [ $# -ne 0 ]; do

            echo "$arg" | ./42sh 1> /dev/null 2> /dev/null
            cat "$1" > output1
            echo "$arg" | bash --posix 1> /dev/null 2> /dev/null

            diff output1 "$1" > /dev/null

            if [ $? -ne 0 ]; then
                flag=1

                out1=$(cat -e output1)
                out2=$(cat -e "$1")
                echo -e "  ${RED}ERROR${NC} IN TEST : \"$testname\"\n  Type: STDIN\n  redirect in $1: ${GREEN}expected :${NC}\n$out2\n  ${RED}got :${NC}\n$out1"
            fi
            shift 1
        done
    fi
    if [ $flag -ne 0 ];then
        return 1
    fi
    return 0
}

test_file() {
    echo "$1" > .file

    ./42sh .file > output1 2> /dev/null
    code1=$?
    bash --posix .file > output2 2> /dev/null
    code2=$?

    flag=0
    diff output1 output2 > /dev/null

    if [ $? -eq 1 ] || [ "$code1" -ne "$code2" ]; then
        flag=1
        out1=$(cat -e output1)
        out2=$(cat -e output2)
        echo -e "  ${RED}ERROR${NC} IN TEST : \"$2\"\n  Type: FILE\n  Print: ${GREEN}expected :${NC}\n$out2\n  ${RED}got :${NC}\n$out1"
        echo -e "  Return Code: ${GREEN}expected :${NC}  $code2  ${RED}got :${NC}  $code1\n"
    fi
    if [ $# -ne 2 ]; then
        arg="$1"
        testname="$2"
        shift 2
        while [ $# -ne 0 ]; do

            ./42sh .file 1> /dev/null 2> /dev/null
            cat "$1" > output1
            bash --posix .file 1> /dev/null 2> /dev/null

            diff output1 "$1" > /dev/null

            if [ $? -ne 0 ]; then
                flag=1

                out1=$(cat -e output1)
                out2=$(cat -e "$1")
                echo -e "  ${RED}ERROR${NC} IN TEST : \"$testname\"\n  Type: FILE\n  redirect in $1: ${GREEN}expected :${NC}\n$out2\n  ${RED}got :${NC}\n$out1"
            fi
            shift 1
        done
    fi
    if [ $flag -ne 0 ];then
        return 1
    fi
    return 0
}

test_string() {
    ./42sh -c "$1" > output1 2> /dev/null
    code1=$?
    bash --posix -c "$1" > output2 2> /dev/null
    code2=$?

    flag=0
    diff output1 output2 > /dev/null

    if [ $? -ne 0 ] || [ "$code1" -ne "$code2" ]; then
        flag=1
        out1=$(cat -e output1)
        out2=$(cat -e output2)
        echo -e "  ${RED}ERROR${NC} IN TEST : \"$2\"\n  Type: STRING\n  Print: ${GREEN}expected :${NC}\n$out2\n  ${RED}got :${NC}\n$out1"
        echo -e "  Return Code: ${GREEN}expected :${NC}  $code2  ${RED}got :${NC}  $code1\n"
    fi
    if [ $# -ne 2 ]; then
        arg="$1"
        testname="$2"
        shift 2
        while [ $# -ne 0 ]; do

            ./42sh -c "$arg" 1> /dev/null 2> /dev/null
            cat "$1" > output1
            bash --posix -c "$arg" 1> /dev/null 2> /dev/null

            diff output1 "$1" > /dev/null

            if [ $? -ne 0 ]; then
                flag=1

                out1=$(cat -e output1)
                out2=$(cat -e "$1")
                echo -e "  ${RED}ERROR${NC} IN TEST : \"$testname\"\n  Type: STRING\n  redirect in $1: ${GREEN}expected :${NC}\n$out2\n  ${RED}got :${NC}\n$out1"
            fi
            shift 1
        done
    fi
    if [ $flag -ne 0 ];then
        return 1
    fi
    return 0
}

test_all() {
    echo "RUNNING TEST : $2..."
    test_string "$@"
    one=$?
    test_file "$@"
    two=$?
    test_stdin "$@"
    three=$?

    if [ $(($one + $two + $three)) -eq 0 ]; then
        pass=$(($pass + 1))
    else
        fail=$(($fail + 1))
    fi
}
# LIST OF FUNCTIONAL TESTS
#
# .test output1 output2 are reserved for the function
# do not use them
#
# exemple :
#   test_all "argument" "name_of_test" "redirect_file_1" "redirect_file_2"
#
# you should only provide redirect_file if your argument make a redirect in a file

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
test_all "echo a | cat -e" "easy pipe"
test_all "ls | cat -e | cat -e" "normal pipe"
test_all "tree | cat -e | grep 42sh" "hard pipe"
test_all "echo ab c |" "error pipe"
test_all "true && false || true" "and_or1"
test_all "false || ! false && true" "and_or_neg"
test_all "echo a\"bcd\"e" "simple double quote"
test_all "echo\" a\"b" "tricky double quote error"
test_all "\\if true; then echo a; fi" "escaping if"
test_all "if true; \\then echo a; fi" "escaping then"
test_all "if true; then echo a\\; fi try me; else echo b; fi" "escaping semicolon"
test_all "echo a\
echo b" "escaping \\\\n"
test_all ";" "single semicolon"
test_all "while false; do echo a; done" "simple while"
test_all "touch test1; touch test2; while diff test1 test2; do echo a; rm test1; done; rm test2" "while only once"
test_all "echo a && echo b || echo c" "and or"
test_all "echo a&&ls" "and no space"
test_all "echo a||ls; ls" "or no space"
test_all "echo a || echo b && echo c && echo d || echo e || echo f && echo g" "a lot of and ors"
test_all "false || ! true && ! echo a && echo b || ls && ! false || echo c && ! echo d || ls" "a lot of and_ors and negs"
test_all "echo a > file" "simple output redirect" "file"
test_all "cat -e < test.sh" "simple input redirect"
test_all 'echo "\""' "escaping double quote in double quotes"
test_all 'a=test; echo "$a"$a' "variables in and out of double quotes"
test_all 'a=test; echo '"'$a'"'$a' "variables in and out of single quotes"
test_all 'a=test echo $a' "environment variable test with echo"
test_all 'TREE_CHARSET=abcd tree' "environment variable test with tree"
test_all 'a=test; echo ${a}' "variable with brackets"
test_all 'false; echo $?' 'test $? with builtin'
test_all 'ls ls; echo $?' 'test $? with executable'
test_all 'lss; echo $?' 'test $? with executable not found'
test_all 'a=b b=c > test; echo $a$b' 'test two variables set' 'test'
test_all 'i\f true; then echo a; fi' 'test i\f'
test_all 'lss >test 1>&2' 'test >& part one' 'test'
test_all 'lss 1>&2 > test' 'test >& part two' 'test'
test_all 'abc=ABC; def=DEF; echo 123$abc$def456; unset abc def; echo 123$abc$def456' 'test unset with simple variable'
#test_all 'abc=ABC; def=DEF; echo avant $abc $def; unset abc def; echo apres $abc $def' 'test unset with simple variable not working'
test_all '. ./test.sh' 'dot with /'
test_all 'PATH=. . test.sh' 'dot using PATH'
test_all '{ echo a; echo b; } | tr b h' 'test raw compound list'
test_all '{echo a; echo b; } | tr b h' 'error 1 compound list'
test_all '{ echo a; echo b } | tr b h' 'error 2 compound list'
test_all 'echo ! a' '! is not a reserved word moment'
test_all 'a=; echo $a' 'variable set to nothing'
#test_all 'test(){ ls;}; test; test; test' 'testing basic function'
#test_all 'test(          )if true; then echo a; else echo b; fi' 'slightly harder function'
test_all 'if(){ echo b;};' 'incorrect function name 1'
test_all 'i\f(){ echo b;};' 'incorrect function name 2'
#test_all 'for i in  "asasasasa" ; do echo $i ; done' "for"

echo -e "${GREEN}Passed: $pass ${NC}, ${RED}Failed $fail${NC}"
rm output1 output2 file .file 2> /dev/null > /dev/null


if [ "$fail" -eq 0 ]; then
    exit 0
fi

exit 1

