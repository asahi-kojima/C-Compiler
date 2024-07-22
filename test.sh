#!/bin/bash

assert(){
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    gcc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo -e "$input => $actual"
    else
        echo -e "$input => $expected expected, but got $actual\n"
        exit 1
    fi
}

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 14 "5 + 20 - 11;"
assert 100 "5 * 2 * 10;"
assert 10 "100 / 10;"
assert 5 "+5;"
assert 1 "4 + -3;"
assert 1 "1 == 1;"
assert 0 "1 != 1;"
assert 1 "3 <= 4 ;"
assert 1 "4 >= 3;"
assert 1 "1 < 100;"
assert 1 "100 > 2;"
assert 0 "1 == 0;"
assert 1 "1 != 0;"
assert 0 "3 <= 2 ;"
assert 0 "-1 >= 3;"
assert 0 "111 < 100;"
assert 0 "100 > 200;"
assert 47 "5 + 6 * 7;"
assert 15 "5 * (9 - 6);"
assert 4 "(3 + 5) / 2;"
assert 0 "(10 * 2 + (10 / 2)) != 25;"
assert 0 "1;0;"
assert 1 "a = 1;"
assert 2 "a = 1;b = 2;"
assert 3 "a = 1;b = 2;c = 3;"
assert 3 "a = 1;aa = 2;aaa = 3;"
assert 2 "a = 1; a + 1;"
assert 123 "b = 456; a = b = 123;"
assert 123 "1;return 123; 1;"
assert 123 "1;return 123;return 100;  1;"
assert 111 "if (1 == 1) 111;"
assert 123 "if (1 == 1) 123; else 55;"
assert 44 "if (1 == 0) 123; else 44;"
assert 123 "x = 1;if (x == 1) 123; else 44;"
assert 123 "x = 1; y = x; if (x == y) 123; else 44;"
assert 10 "x = 1; while(x < 10) x = x + 1;"
assert 10 "x = 1; for (x = 1; x < 10; 1 == 1) x = x + 1; x; "

echo OK