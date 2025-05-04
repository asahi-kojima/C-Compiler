#!/usr/bin/bash

assert(){
    echo "========================================================"

    expected="$1"
    input="$2"

    echo "now compiling..."
    ./compiler.out "$input" > tmp.s

    echo "compile finish!"
    gcc -o tmp -z noexecstack tmp.s

    if [ "$?" -ne 0 ]; then
        echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
        echo "Error : failed to convert object file from assembly file!"
        echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"

        echo "--- assembly file ---"
        cat ./tmp.s
        exit 1
    fi
    ./tmp
    actual="$?"
    
    if [ "$actual" = "$expected" ]; then
        echo -e "OK : $input => $actual"
    else
        echo -e "Error : $input => $expected expected, but got $actual\n"
        cat ./tmp.s
        exit 1
    fi
}

assert 0 "0"
assert 1 "1"
assert 2 "1 + 1"
assert 0 "123 - 123"