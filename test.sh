#!/usr/bin/bash

assert(){
    echo "========================================================"

    expected="$1"
    input="$2"

    ./compiler.out "$input" > tmp.s
    if [ "$?" -ne 0 ]; then
        echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
        echo "Error : failed to compile your code!"
        echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
        exit 1
    fi

    gcc -o tmp -z noexecstack tmp.s

    if [ "$?" -ne 0 ]; then
        echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
        echo "Error : failed to convert object file from assembly file!"
        echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"

        echo "--- assembly file ---"
        cat ./tmp.s
        echo "---------------------"
        exit 1
    fi
    ./tmp
    actual="$?"
    
    if [ "$actual" = "$expected" ]; then
        echo -e "OK : $input => $actual"
    else
        echo -e "Error : $input => $expected expected, but got $actual"
        exit 1
    fi
}

#四則演算のテスト
assert 0 "0"
assert 1 "1"
assert 2 "1 + 1"
assert 0 "123 - 123"
assert 3 "1 + 2 + 3 - 2 - 1"
assert 7 "1 + (2 * 3)"
assert 5 "(3 * 3) -  (2 * 2)"
assert 10 "100 / 10"
assert 8 "((10 * 20) / 2 - 20) / 10"

#単項演算子のテスト
assert 1 "+1"
assert 2 "1 - -1"
assert 2 "+2 * +1"
assert 12 "-4 * -3"
assert 16 "(-4 * -3) - -4"

echo "========================================================"