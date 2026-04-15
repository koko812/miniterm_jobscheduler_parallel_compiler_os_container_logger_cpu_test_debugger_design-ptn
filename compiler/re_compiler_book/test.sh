#!/bin/bash
#input="1 + (2*3+4) * (3-1+2) - --- 4 "
input="za=1; b=(za+2)*3; return za+3*b; 3+3*2;"
./mini_cc "$input" > temp.s

printf "<asm>\n"
cat temp.s
gcc -o temps temp.s

printf "\n<result>\n"
./temps
echo "$?"