#!/usr/bin/bash

BUILD_DIR=build

assert(){
    echo "========================================================"

    expected="$1"
    input="$2"

    ${BUILD_DIR}/compiler.out "$input" > ${BUILD_DIR}/tmp.s
    is_success="$?"
    if [ $is_success -ne 0 ]; then
        echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
        echo "Error $is_success: failed to compile your code!"
        echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
        exit 1
    fi

    gcc -o ${BUILD_DIR}/tmp.out -z noexecstack ${BUILD_DIR}/tmp.s

    if [ "$?" -ne 0 ]; then
        echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
        echo "Error : failed to convert object file from assembly file!"
        echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"

        echo "--- assembly file ---"
        cat ./tmp.s
        echo "---------------------"
        exit 1
    fi
    
    ${BUILD_DIR}/tmp.out
    actual="$?"
    
    if [ "$actual" = "$expected" ]; then
        echo -e "OK : $input => $actual"
    else
        echo -e "Error : $input => $expected expected, but got $actual"
        exit 1
    fi
}

#四則演算のテスト
assert 0 "0;"
assert 1 "1;"
assert 2 "1 + 1;"
assert 0 "123 - 123;"
assert 3 "1 + 2 + 3 - 2 - 1;"
assert 7 "1 + (2 * 3);"
assert 5 "(3 * 3) -  (2 * 2);"
assert 10 "100 / 10;"
assert 8 "((10 * 20) / 2 - 20) / 10;"

#単項演算子のテスト
assert 1 "+1;"
assert 2 "1 - -1;"
assert 2 "+2 * +1;"
assert 12 "-4 * -3;"
assert 16 "(-4 * -3) - -4;"

#比較演算子のテスト
assert 1 "1 == 1;"
assert 0 "1 == 0;"
assert 1 "1 != 0;"
assert 0 "1 != 1;"

assert 1 "(2 * 2) == (2 + 2);"
assert 0 "2 == (1 / 1);"
assert 1 "(2 * 2) != 0;"
assert 0 "(3 * 3) != (3 + 3 + 3);"

assert 1 "0 < 1;"
assert 1 "0 <= 1;"
assert 1 "1 <= 1;"
assert 0 "2 < 1;"
assert 0 "2 <= 1;"

assert 1 "-1 < -0;"
assert 1 "-1 <= -0;"
assert 1 "-1 <= -1;"
assert 0 "-1 < -2;"
assert 0 "-1 <= -2;"

assert 1 "1 > 0;"
assert 1 "1 >= 0;"
assert 1 "1 >= 1;"
assert 0 "1 > 2;"
assert 0 "1 >= 2;"

assert 1 "-0 > -1;"
assert 1 "-0 >= -1;"
assert 1 "-1 >= -1;"
assert 0 "-2 > -1;"
assert 0 "-2 >= -1;"

#複数の文がある場合のテスト
assert 0 "0;0;"
assert 1 "1;1;1;"
assert 123 "1 + 1; (3 + 3) * (3 / 3); 100 + 20 + 3;"

#変数のテスト
assert 0 "x = 0;"
assert 1 "y = x = 1;"
assert 1 "y = x = 1;y;"
assert 2 "x = 1; y = x + 1;"
assert 3 "x = 1; y = 2; z = x + y;"
assert 7 "x = 1; y = 2; z = x + y; 2 * z + 1;"

echo "========================================================"