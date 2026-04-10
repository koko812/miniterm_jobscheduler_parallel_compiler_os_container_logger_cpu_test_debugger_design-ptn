#!/bin/bash
# set -x
assert(){
    expected="$1"
    input="$2"

    ./dentaku "$input" > temp.s
    gcc -o temps temp.s ext.o
    ./temps
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

# assert 0 "0;"
# assert 42 "42;"
#assert 21 20+5-4;
#assert 5 1+2+3+4-5
#assert 5 "1+2   + 3 + 4-5"
#assert 5 "1 + 2 + 3 + 4 - 5"
#assert 58 "12 + 23 + 34 + 45 - 56"
#assert 58 " 12 +       23 + 34 + 45 - 56"
#assert 93 "123 - 23 + 4 + 45 - 56"
#assert 12 "3*4"
#ssert 13 "3*4+2/2"
# assert 16 "2+3*4+8/2-2"
# assert 10 "(1+1)*5"
# assert 6 "1+1*5"
# assert 13 "(2+4)*3 - 3*(1+2) + 4"
# assert 1 "-2 + 3"
# assert 1 "1<2"
# assert 0 "1>2"
# assert 0 "3<2"
# assert 1 "3>2"
# assert 1 "3>=3"
# assert 0 "3>3"
# assert 0 "3==2"
# assert 1 "3==3"
# assert 1 "( 3 + 2 ) * 4 == 2 * 10"
# assert 0 "( 3 + 2 ) * 4 != 2 * 10"
# assert 1 "( 3 + 2 ) * 3 != 2 * 10;"
# assert 3 "a=1;b=2;a+b;"
# assert 3 "abc=1;bcd=2;abc+bcd;"
# assert 5 "x=1; y=(x+1)*2; y+1;"
# assert 7 "x=1; y=(x+1)*2+1; div=5; z=y/div; z+y+1;"
# assert 5 "return 5;"
# assert 5 "returnx=5; return returnx;"
# assert 7 "x=2;y=3;return x+x+y; x=x+2; return x;"
# assert 1 "if (1) return 1; else return 2;"
# assert 2 "if (0) return 1; else return 2;"
# assert 4 "a=3; b=7; if (a==b) return 0; if (a<b) return b-a; else return a-b;"
# assert 113 "a=1;b=2;c=3;d=4;e=5; while(a<100) a=a+b+c+d+e; return a;"
# assert 10 "for(i=1; i<10; i=i+1) i+2; return i;"
# assert 20 "i=0; a=0; while(i<10){i=i+1; a=a+2;} return a;";
# assert 55 "i=0; a=0; while(i<10){i=i+1; a=a+i;} return a;";
# assert 155 "i=0; a=0; while(i<20){i=i+1; if(i>10){a=a+i;}} return a;";
# assert 42 "foo();"
# assert 5 "add(2,3);"
# assert 243 "powe(3,5);"
# assert 3 "main(){return 3;}"
# assert 7 "add(x,y){ return x+y; } main(){return add(3,4);}"
# assert 120 "fact(n){ if(n<2) return 1; return n*fact(n-1); } main(){ return fact(5); }"
# assert 55 "fib(n){ if(n<2) return n; return fib(n-1)+fib(n-2); } main(){ return fib(10); }"
# assert 3 "main(){a=3; p=&a; return *p;}"
# assert 5 "main(){a=3; p=&a; *p=5; return a;}"
# assert 7 "main(){a=7; return *(&a);}"
assert 3 "int main(){ int a; a=3; return a; }"
assert 120 "int fact(int n){ if(n<2) return 1; return n*fact(n-1);} int main(){ return fact(5); }"
# assert 3 "int main(){ a=3; return a; }"
assert 3 "int main(){ int a; int *p; a=3; p=&a; return *p; }"
assert 5 "int main(){ int a; int *p; a=3; p=&a; *p=5; return a; }"
assert 7 "int main(){ int a; int *p; int **pp; a=7; p=&a; pp=&p; return **pp; }"
assert 9 "int add1(int *p){ return *p + 1; } int main(){ int a; a=8; return add1(&a); }"
#assert 1 "int main(){ int a = 0; int *p; p=3; return *p; }"
#assert 1 "int main(){ int a; a = 0; int *p; p=&a; return p; } "
assert 30 "int main(){ int *p; p=alloc3(); return *(p+2); }"
assert 4 "int main(){ int a; return sizeof(a); }"
assert 4 "int main(){ int a; return sizeof(a+3); }"
assert 8 "int main(){ int *p; return sizeof(p); }"
assert 4 "int main(){ int *p; return sizeof(*p); }"
assert 4 "int main(){ return sizeof(1); }"
assert 4 "int main(){ return sizeof(sizeof(1)); }"
assert 3 "int main(){ int a[2]; *a=1; *(a+1)=2; return *a+*(a+1); }"
assert 3 "int main(){ int a[2]; *a=1; *(a+1)=2; int *p; p=a; return *p+*(p+1); }"
assert 3 "int main(){ int a[2]; *a=1; *(a+1)=2; return a[0]+a[1]; }"
assert 5 "int main(){ int a[2]; a[0]=2; a[1]=3; return a[0]+a[1]; }"
assert 3 "int g; int main(){ g=3; return g; }"



echo OK