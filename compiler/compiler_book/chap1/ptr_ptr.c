int main() {
    int x = 10;
    int *p = &x;
    int **pp = &p;
    return **pp;
}