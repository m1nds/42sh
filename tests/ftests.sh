#!/bin/sh

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

pass=0
fail=0

test_stdin() {
    echo "$1" | ./42sh > output1 2> /dev/null
    echo "$1" | bash --posix > output2 2> /dev/null

    flag=0

    out1=$(cat output1)
    out2=$(cat output2)

    echo "$1" | ./42sh 2> /dev/null
    code1=$?
    echo "$1" | bash --posix 2> /dev/null
    code2=$?

    diff output1 output2
    check=$?

    if [ ! "$check" ]; then
        echo -e "  ${RED}ERROR${NC} Type: STDIN\n  Print $2: expected :\n  $out2\n  got :\n  $out1"
        $flag=1
    fi

    if [ "$code1" -ne "$code2" ]; then
        echo -e "  ${RED}ERROR${NC} Type: STDIN\n  Return CODE $2: expected :\n  $out2\n  got :\n  $out1"
        return 1
    fi

    if [ "$flag" -ne "1" ]; then
        echo -e "  STDIN Test \"$2\" : ${GREEN}OK${NC}"
        return 0
    fi

    return 1
}

test_file() {
    echo "$1" > file

    ./42sh file > output1 2> /dev/null
    bash --posix file > output2 2> /dev/null

    flag=0

    out1=$(cat output1)
    out2=$(cat output2)

    ./42sh file 2> /dev/null
    code1=$?
    bash --posix file 2> /dev/null
    code2=$?

    diff output1 output2
    check=$?

    if [ ! "$check" ]; then
        echo -e "  ${RED}ERROR${NC} Type: FILE\n  Print: expected :\n  $out2\n  got :\n$out1"
        $flag=1
    fi

    if [ "$code1" -ne "$code2" ]; then
        echo -e "  ${RED}ERROR${NC} Type: FILE\n  Return CODE: expected :\n  $out2\n  got :\n$out1"
        return 1
    fi

    if [ "$flag" -ne "1" ]; then
        echo -e "  FILE   Test \"$2\" : ${GREEN}OK${NC}"
        return 0
    fi

    return 1
}

test_string() {
    ./42sh -c "$1"> output1 2> /dev/null
    bash --posix -c "$1" > output2 2> /dev/null

    flag=0

    out1=$(cat output1)
    out2=$(cat output2)

    ./42sh -c "$1" 2> /dev/null
    code1=$?
    bash --posix -c "$1" 2> /dev/null
    code2=$?

    diff output1 output2
    check=$?

    if [ ! "$check" ]; then
        echo -e "  ${RED}ERROR${NC} Type: STRING\n  Print $2: expected :\n  $out2\n  got :\n  $out1"
        $flag=1
    fi

    if [ "$code1" -ne "$code2" ]; then
        echo -e "  ${RED}ERROR${NC} Type: STRING\n  Return CODE $2: expected :\n  $out2\n  got :\n  $out1"
        return 1
    fi

    if [ "$flag" -ne "1" ]; then
        echo -e "  STRING Test \"$2\" : ${GREEN}OK${NC}"
        return 0
    fi

    return 1
}

test_all() {
    echo "Running \"$2\"..."

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

echo -e "${GREEN}Passed: $pass ${NC}, ${RED}Failed $fail${NC}"

if [ "$fail" -eq 0 ]; then
    exit 0
fi

exit 1

