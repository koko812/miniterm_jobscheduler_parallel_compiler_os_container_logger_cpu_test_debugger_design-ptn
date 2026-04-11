int adds(int a, int b){
    return a+b;
}

int power_fn(int a, int n){
    if(n==1){
        return a;
    }
    return power_fn(a, n-1)*a;
}

int a;
int main(){
    // this is a comment
    int b;
    a=2; b=3;
    /* this shoud be 
    ignored by my compiler */
    return power_fn(a,b);
}
