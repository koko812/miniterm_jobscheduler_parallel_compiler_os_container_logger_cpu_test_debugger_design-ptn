#include "dentaku.h"
#include <stdio.h>

int main(int argc, char **argv){
    if (argc != 2){
        fprintf(stderr, "引数の数が正しくありません");
        return 1;
    }

    user_input = argv[1];
    token = tokenize(user_input);
    Node *node = expr();

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    gen(node);

    printf("  pop rax\n");
    printf("  ret\n\n");
    printf(".section .note.GNU-stack,\"\",@progbits\n");

    return 0;
}