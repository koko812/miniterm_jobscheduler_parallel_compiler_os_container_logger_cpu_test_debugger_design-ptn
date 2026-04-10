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

    printf(".data\n");
    for(GVar *g = globals; g; g=g->next){
        printf(".globl %.*s\n", g->len, g->name);
        printf("%.*s:\n", g->len, g->name);
        printf("  .zero %d\n", size_of(g->ty));
    }

    for(GVar *g = strings; g; g=g->next){
        printf("%.*s:\n", g->len, g->name);
        for (int i=0; i<g->init_len; i++)
            printf("  .byte %d\n", (unsigned char)g->init_data[i]);
        printf("  .byte 0\n");
    }
    printf(".text\n");

    for(int i=0; funcs[i]; i++){
        add_type(funcs[i]->body, true);
        gen_func(funcs[i]);
    }

    printf(".section .note.GNU-stack,\"\",@progbits\n");

    return 0;
}