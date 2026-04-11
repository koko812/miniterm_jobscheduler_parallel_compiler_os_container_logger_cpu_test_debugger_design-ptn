int assert(int expected, int actual){
    if (expected == actual){
        printf("%d => %d\n", expected, actual);
        return 0;
    }
    printf("%d => %d (NG)\n", expected, actual);
    return 1;
}

int add(int x, int y){
    return x + y;
}

int fib(int n){
    if (n < 2) return n;
    return fib(n-1) + fib(n-2);
}

int main(){
    int fails;
    fails = 0;

    fails = fails + assert(3, 1+2);
    fails = fails + assert(7, add(3,4));
    fails = fails + assert(55, fib(10));

    if (fails != 0) return 1;
    return 0;
}
