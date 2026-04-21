#include <stdlib.h>

int foo(int a, int b){return a+b;}
void alloc4(int **p, int a, int b, int c, int d) {
    *p = malloc(sizeof(int) * 4);
    (*p)[0] = a;
    (*p)[1] = b;
    (*p)[2] = c;
    (*p)[3] = d;
}
