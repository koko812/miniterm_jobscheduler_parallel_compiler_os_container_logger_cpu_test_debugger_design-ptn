#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

char *user_input;

typedef enum{
    TK_RESERVED,
    TK_NUM,
    TK_EOF,
} Tokenkind;

static const char *tk_kind_name[] = {
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


typedef enum{
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
} Nodekind;

static const char *nd_kind_name[] = {
    "ND_ADD",
    "ND_SUB",
    "ND_MUL",
    "ND_DIV",
    "ND_NUM",
};

typedef struct Node Node;
struct Node{
    Nodekind kind;
    Node *lhs;
    Node *rhs;
    int val;
};


void error(){
    fprintf(stderr, "間違ってます\n");
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

        fprintf(stderr, "[ERROR]: failed to tokenize.\n");
        exit(1);
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}

void dump_tokens(){
    fprintf(stderr, "%s\n", user_input);
    Token *tok = head.next;
    while(tok){
        fprintf(stderr, "%-15s \'%.*s\'   val=%5d\n",
                tk_kind_name[tok->kind], 1, tok->str, tok->val);
        tok = tok->next;
    }
}


Node *new_node(Nodekind kind, Node *lhs, Node *rhs){
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val){
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Node *num(){
    return new_node_num(expect_num());
}

Node *expr(){
    Node *node = num();

    for(;;){
        if(consume('+')){
            node = new_node(ND_ADD, node, num());
        }
        else if(consume('-')){
            node = new_node(ND_SUB, node, num());
        }else{
            return node;
        }
    }
}

void dump_ast_indent(Node *node, int depth){
    fprintf(stderr, "%*.s", depth, " ");
    if(node->kind == ND_NUM){
        fprintf(stderr, "%-8s %d\n", nd_kind_name[node->kind], node->val);
        return;
    }
    fprintf(stderr, "%-8s\n", nd_kind_name[node->kind]);
    dump_ast_indent(node->lhs, depth+1);
    dump_ast_indent(node->rhs, depth+1);
}

void dump_ast_prefix(Node *node, int depth){
}


void gen(Node *node){
    if(node->kind==ND_NUM){
        printf("  push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind){
        case ND_ADD:
            printf("  add rax, rdi\n");
            break;
        case ND_SUB:
            printf("  sub rax, rdi\n");
            break;
        default:
            break;
    }

    printf("  push rax\n");
}


int main(int argc, char **argv){
    if(argc != 2){
        printf("引数の数が正しくありません．");
        return 1;
    }

    user_input = argv[1];
    token = tokenize();
    fprintf(stderr, "\n\n<Tokens>\n");
    dump_tokens();

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    Node *node = expr();
    gen(node);

    fprintf(stderr, "\n\n<AST>\n");
    dump_ast_indent(node, 0);
    fprintf(stderr, "\n");

    printf("  pop rax\n");
    printf("  ret\n");
    printf(".section .note.GNU-stack,\"\",@progbits\n");
}