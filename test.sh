#!/bin/bash

assert(){
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    gcc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 0
assert 42 42
assert 21 "5+20-4"
assert 14 "5 + 20 - 11"
assert 100 "5 * 2 * 10"
assert 10 "100 / 10"
assert 5 "+5"
assert 1 "4 + -3"
assert 1 " 1 == 1"
assert 0 " 1 != 1"
assert 1 "3 <= 4 "
assert 1 "4 >= 3"
assert 1 "1 < 100"
assert 1 "100 > 2"
assert 0 " 1 == 0"
assert 1 " 1 != 0"
assert 0 "3 <= 2 "
assert 0 "-1 >= 3"
assert 0 "111 < 100"
assert 0 "100 > 200"
assert 0 "(10 * 2 + (10 / 2)) != 25"

echo OK