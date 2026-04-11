#!/bin/bash
# set -x
assert(){
    expected="$1"
    input="$2"

    ./dentaku "$input" > temp.s
    gcc -o temps temp.s
    ./temps
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 3 test1.c

echo OK