#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

char *user_input;
void codegen();
void one_plus_one();

typedef enum{
    TK_RESERVED,
    TK_NUM,
    TK_EOF,
} Tokenkind;

typedef struct Token Token;
struct Token{
    Tokenkind kind;
    Token *next;
    int val;
    char *str;
};

Token *token;


void retval(){
    printf("  mov rax, %s\n", user_input);
}

int adder(){
    char *p = user_input;
    printf("  mov rax, %ld\n", strtol(p, &p, 10));
    while(*p){
        if(isspace(*p)){
            p++;
            continue;
        }
        if(*p=='+'){
            p++;
            printf("  add rax, %ld\n", strtol(p, &p, 10));
            continue;
        }
        if(*p=='-'){
            p++;
            printf("  sub rax, %ld\n", strtol(p, &p, 10));
            continue;
        }

        fprintf(stderr, "予期しない文字です．");
        return 1;
    }
}


int main(int argc, char **argv){
    if(argc != 2){
        printf("引数の数が正しくありません．");
        return 1;
    }

    user_input = argv[1];

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");
    // one_plus_one();
    // codegen();
    int err = adder();
    if(err) return 1;
    printf("  ret\n");
    printf(".section .note.GNU-stack,\"\",@progbits\n");
}


void one_plus_one(){
    printf("  mov rax, 1\n");
    printf("  add rax, 1\n");
}

void codegen(){
}