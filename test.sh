#!/usr/bin/bash

assert(){
    echo "========================================================"

    expected="$1"
    input="$2"

    ./compiler.out "$input" > tmp.s
    gcc -o tmp tmp.s
    ./tmp
    actual="$?"
    
    if [ "$actual" = "$expected" ]; then
        echo -e "OK : $input => $actual"
    else
        echo -e "$input => $expected expected, but got $actual\n"
        exit 1
    fi
}

assert 0 "0"