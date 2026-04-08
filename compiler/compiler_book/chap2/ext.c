int foo() { return 42; }
int add(int x, int y) { return x + y; }
int powe(int x, int p) {int xx=x; for(int i=0;i<p-1;i++)x=x*xx;return x;}
int *alloc3() {
    static int buf[3];
    buf[0] = 10;
    buf[1] = 20;
    buf[2] = 30;
    return buf;
}
