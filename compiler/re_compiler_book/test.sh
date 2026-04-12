#!/bin/bash
input="1+1+1-1+1 -1"
./mini_cc "$input" > temp.s

printf "<asm>\n"
cat temp.s
gcc -o temps temp.s

printf "\n<result>\n"
./temps
echo "$?"