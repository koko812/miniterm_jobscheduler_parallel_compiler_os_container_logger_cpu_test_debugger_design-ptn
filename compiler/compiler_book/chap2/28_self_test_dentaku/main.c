#include "dentaku.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

char *filename;
int main(int argc, char **argv){
    if (argc != 2){
        fprintf(stderr, "引数の数が正しくありません");
        return 1;
    }

    filename = argv[1];
    user_input = read_file(argv[1]);
    tokenize();
    fprintf(stderr, "tokenize ok\n");


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


char *read_file(char *path){
    FILE *fp = fopen(path, "r");
    if(!fp)
        error("cannot open %s: %s", path, strerror(errno));

    if(fseek(fp, 0, SEEK_END) == -1)
        error("%s: fseek: %s", path, strerror(errno));
    size_t size = ftell(fp);
    if(fseek(fp, 0, SEEK_SET) == -1)
        error("%s: fseek: %s", path, strerror(errno));

    char *buf = calloc(1, size+2);
    fread(buf, size, 1, fp);

    if(size==0 || buf[size-1]!='\n')
        buf[size++] = '\n';
    
    buf[size] = '\0';
    fclose(fp);
    return buf;
}