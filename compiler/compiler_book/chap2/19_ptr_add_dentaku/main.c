#include "dentaku.h"
#include <stdio.h>

int main(int argc, char **argv){
    if (argc != 2){
        fprintf(stderr, "引数の数が正しくありません");
        return 1;
    }

    user_input = argv[1];
    tokenize();

    printf(".intel_syntax noprefix\n");

    program();
    for(int i=0; funcs[i]; i++){
        add_type(funcs[i]->body);
        gen_func(funcs[i]);
    }

    printf(".section .note.GNU-stack,\"\",@progbits\n");

    return 0;
}