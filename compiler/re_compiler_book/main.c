#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

char *user_input;
void codegen();
void one_plus_one();

typedef enum{
    TK_RESERVED,
    TK_NUM,
    TK_EOF,
} Tokenkind;

static const char *kind_name[] = {
    "TK_RESERVED",
    "TK_NUM",
    "TK_EOF",
};


typedef struct Token Token;
struct Token{
    Tokenkind kind;
    Token *next;
    int val;
    char *str;
};

Token *token;

void error(){
    printf("間違ってます");
    exit(1);
}

bool consume(char op){
    if(token->kind!=TK_RESERVED || token->str[0]!=op)
        return false;
    token = token->next;
    return true;
}

bool consume_num(char op){
}

Token *expect(char op){
}

int expect_num(){
    if(token->kind!=TK_NUM){
        error();
    }
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof(){
    return token->kind == TK_EOF;
}

Token *new_token(Tokenkind kind, Token *cur, char *str){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;

    return tok;
}

Token head;
Token *tokenize(){
    head.next = NULL;
    Token *cur = &head;
    char *p = user_input;

    while(*p){
        if(isspace(*p)){
            p++;
            continue;
        }
        if(*p=='+' || *p=='-'){
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }
        if(isdigit(*p)){
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        fprintf(stderr, "failed to tokenize.");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}

void dump_tokens(){
    fprintf(stderr, "%s\n", user_input);
    Token *tok = head.next;
    while(tok){
        fprintf(stderr, "%-15s \'%.*s\'   val=%5d\n",
                kind_name[tok->kind], 1, tok->str, tok->val);
        tok = tok->next;
    }
}

void retval(){
    printf("  mov rax, %s\n", user_input);
}

void gen(){
    printf("  mov rax, %d\n", expect_num());

    while(!at_eof()){
        if(consume('+')){
            printf("  add rax, %d\n", expect_num());
            continue;
        }
        if(consume('-')){
            printf("  sub rax, %d\n", expect_num());
            continue;
        }

        fprintf(stderr, "予期しない文字です．");
    }
}


int main(int argc, char **argv){
    if(argc != 2){
        printf("引数の数が正しくありません．");
        return 1;
    }

    user_input = argv[1];
    token = tokenize();
    dump_tokens();

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    gen();

    printf("  ret\n");
    printf(".section .note.GNU-stack,\"\",@progbits\n");
}


void one_plus_one(){
    printf("  mov rax, 1\n");
    printf("  add rax, 1\n");
}

void codegen(){
}