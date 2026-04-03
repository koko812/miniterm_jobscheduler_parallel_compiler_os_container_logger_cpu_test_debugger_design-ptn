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
#assert 21 20+5-4
#assert 5 1+2+3+4-5
#assert 5 "1+2   + 3 + 4-5"
#assert 5 "1 + 2 + 3 + 4 - 5"
#assert 58 "12 + 23 + 34 + 45 - 56"
#assert 58 " 12 +       23 + 34 + 45 - 56"
assert 93 "123 - 23 + 4 + 45 - 56"
#assert 12 "3*4"
#ssert 13 "3*4+2/2"
assert 16 "2+3*4+8/2-2"
assert 10 "(1+1)*5"
assert 6 "1+1*5"
assert 13 "(2+4)*3 - 3*(1+2) + 4"
assert 1 "-2 + 3"
assert 1 "1<2"
assert 0 "1>2"
assert 0 "3<2"
assert 1 "3>2"
assert 1 "3>=3"
assert 0 "3>3"

echo OK