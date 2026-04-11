#!/bin/bash
set -e

./dentaku test_all.c > temp.s
gcc -o temps temp.s
./temps
echo OK
