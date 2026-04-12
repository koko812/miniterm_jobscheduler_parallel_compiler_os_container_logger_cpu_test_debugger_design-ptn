#!/bin/bash
input="4-3+1-4+8+1"
./mini_cc "$input" > temp.s

printf "<asm>\n"
cat temp.s
gcc -o temps temp.s

printf "\n<result>\n"
./temps
echo "$?"