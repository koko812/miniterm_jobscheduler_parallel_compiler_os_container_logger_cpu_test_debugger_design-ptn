#!/bin/bash
#input="1 + (2*3+4) * (3-1+2) - --- 4 "
#input="za=1; b=(za+2)*3; return za+3*b; 3+3*2;"
#input="a=1; if(a==1) return 2;"
#input="a=1; while (a<5) a=a+1; if (a>8) return a; else return 0;"
#input="a=1; i=4; for(a=1; a<10; a=a+1) i=i+a; return i;"
#input="a=1; i=4; for(a=1; a<10; a=a+1) {i=i+a; i=i+1;} return i;"
#input="a = foo(1,2); return a;"
#input="adfoo(a,b){c=2; return a+b+c;} main(){a = adfoo(1,2); return a;}"
#input="main(){a = 1; b = &a; *b=5; return a;}"
#input="main(){a = 1; y=x+1; return y;}"
#input="int main(){ int a; a=3; return a; }"
#input="int add(int x, int y){ return x+y; } int main(){ return add(2,3); }"
#input="int main(){int a; a = 1; int b; b = &a; *b=5; return a;}"
input="int set5(int *p){ *p = 5; return 0;} int main(){ int a; a = 1; set5(&a); return a;}"

./mini_cc "$input" > temp.s

printf "<asm>\n"
cat temp.s
gcc -o temps temp.s ext.o

printf "\n<result>\n"
./temps
echo "$?"