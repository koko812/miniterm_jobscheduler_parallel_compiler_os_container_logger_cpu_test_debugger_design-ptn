#!/bin/bash
set -x
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

assert 0 0
assert 42 42
assert 21 20+5-4
assert 5 1+2+3+4-5
assert 5 "1+2   + 3 + 4-5"
assert 5 "1 + 2 + 3 + 4 - 5"
assert 58 "12 + 23 + 34 + 45 - 56"
assert 58 " 12 +       23 + 34 + 45 - 56"
assert 93 "123 - 23 + 4 + 45 - 56"

echo OK