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
        cat ${BUILD_DIR}/tmp.s
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

# testsディレクトリを作成し、その中の.shファイルをすべて読み込む
for test_file in ./tests/*.sh; do
    . "$test_file"
done

echo "========================================================"