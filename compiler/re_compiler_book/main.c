#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#ifdef DEBUG_PARSE
#define TRACE() \
    fprintf(stderr, "%11s:   %-15s '%.*s' val=%5d\n", \
        __func__, tk_kind_name[token->kind], token->len, token->str, token->val)
#else
#define TRACE() ((void)0)
#endif

char *user_input;

typedef enum{
    TK_RESERVED,
    TK_NUM,
    TK_IDENT,
    TK_RETURN,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_EOF,
} Tokenkind;

static const char *tk_kind_name[] = {
    "TK_RESERVED",
    "TK_NUM",
    "TK_IDENT",
    "TK_RETURN",
    "TK_IF",
    "TK_ELSE",
    "TK_WHILE",
    "TK_FOR",
    "TK_EOF",
};

typedef struct Token Token;
struct Token{
    Tokenkind kind;
    Token *next;
    int val;
    char *str;
    int len;
};

Token *token;

typedef struct LVar LVar;
struct LVar{
    LVar *next;
    char *str;
    int len;
    int offset;
};
LVar *locals;


typedef enum{
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_ASSIGN,
    ND_NUM,
    ND_LVAR,
    ND_LEQ,
    ND_LTH,
    ND_EQU,
    ND_NEQ,
    ND_IF,
    ND_WHILE,
    ND_FOR,
    ND_RETURN,
    ND_BLOCK,
    ND_FUNCALL,
} Nodekind;

static const char *nd_kind_name[] = {
    "ND_ADD",
    "ND_SUB",
    "ND_MUL",
    "ND_DIV",
    "ND_ASSIGN",
    "ND_NUM",
    "ND_LVAR",
    "ND_LEQ",
    "ND_LTH",
    "ND_EQU",
    "ND_NEQ",
    "ND_IF",
    "ND_WHILE",
    "ND_FOR",
    "ND_RETURN",
    "ND_BLOCK",
    "ND_FUNCALL",
};

typedef struct Node Node;
struct Node{
    Nodekind kind;
    Node *lhs;
    Node *rhs;
    Node *cond;
    Node *then;
    Node *els;
    Node *init;
    Node *inc;
    Node *body;
    Node *next;
    Node *args;
    LVar *var;
    char *funcname;
    int val;
    int offset;
    int funcname_len;
};

typedef struct Function Function;
struct Function{
    Function *next;
    Node *body;
    LVar *params;
    LVar *locals;
    char *name;
    int name_len;
};

void error(char *s){
    fprintf(stderr, "\n%s\n", s);
    exit(1);
}

bool consume(char *op){
    if(token->kind!=TK_RESERVED || 
        token->len != strlen(op) ||
        memcmp(op, token->str, token->len))
        return false;
    token = token->next;
    return true;
}

bool consume_kw(Tokenkind kind){
    if(token->kind!=kind)
        return false;
    token = token->next;
    return true;
}

bool consume_num(){
}

Token *consume_ident(){
    if(token->kind!=TK_IDENT)
        return false;
    Token *tok = token;
    token = token->next;
    return tok;
}


void *expect(char *op){
    if(token->kind!=TK_RESERVED || 
        token->len != strlen(op) ||
        memcmp(op, token->str, token->len))
        error("expect is missed");
    token = token->next;
}

int expect_num(){
    if(token->kind!=TK_NUM){
        error("expect_num is missed");
    }
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof(){
    return token->kind == TK_EOF;
}

Token *new_token(Tokenkind kind, Token *cur, char *str, int len){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    tok->len = len;

    return tok;
}

int match_keyword(char* p, char* key){
    int len = strlen(key);
    if (strncmp(p, key, len) == 0 && !isalnum(p[len]))
        return len;
    return 0;
}

Token head;
Token *tokenize(){
    head.next = NULL;
    Token *cur = &head;
    char *p = user_input;

    while(*p){
        int len = 0;
        if(isspace(*p)){
            p++;
            continue;
        }

        if((len = match_keyword(p, "if"))){
            cur = new_token(TK_IF, cur, p, len);
            p += len;
            continue;
        }

        if((len = match_keyword(p, "else"))){
            cur = new_token(TK_ELSE, cur, p, len);
            p += len;
            continue;
        }

        if((len = match_keyword(p, "while"))){
            cur = new_token(TK_WHILE, cur, p, len);
            p += len;
            continue;
        }

        if((len = match_keyword(p, "for"))){
            cur = new_token(TK_FOR, cur, p, len);
            p += len;
            continue;
        }

        if((len = match_keyword(p, "return"))){
            cur = new_token(TK_RETURN, cur, p, len);
            p += len;
            continue;
        }

        if(!strncmp(p, "==", 2) ||
            !strncmp(p, "!=", 2) ||
            !strncmp(p, ">=", 2) ||
            !strncmp(p, "<=", 2)){
            cur = new_token(TK_RESERVED, cur, p, 2);
            p = p+2;
            continue;
        }

        if(strchr("+-*/()<>=;{},", *p)){
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if(isdigit(*p)){
            char *start = p;
            cur = new_token(TK_NUM, cur, start, 0);
            cur->val = strtol(p, &p, 10);
            cur->len = p - start;
            continue;
        }

        if(isalpha(*p)){
            char *start = p;
            while(isalnum(*p) || *p == '_') p++;
            int len = p - start;
            cur = new_token(TK_IDENT, cur, start, len);
            continue;
        }

        fprintf(stderr, "[ERROR]: failed to tokenize.\n");
        exit(1);
    }

    new_token(TK_EOF, cur, p, 0);
    return head.next;
}

void dump_tokens(){
    fprintf(stderr, "%s\n", user_input);
    Token *tok = head.next;
    while(tok){
        fprintf(stderr, "%-15s \'%.*s\'   val=%5d\n",
                tk_kind_name[tok->kind], tok->len, tok->str, tok->val);
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

LVar *find_LVar(Token *tok){
    for(LVar *var = locals; var; var=var->next){
        if(tok->len == var->len && !memcmp(tok->str, var->str, var->len))
            return var;
    }
    return NULL;
}

Node *add();
Node *expr();

Node *factor(){
    TRACE();
    if(consume("(")){
        Node *node = add();
        expect(")");
        return node;
    }
    Token *tok = consume_ident();
    if(tok){
        Node *node = calloc(1, sizeof(Node));
        if(consume("(")){
            node->kind = ND_FUNCALL;
            node->funcname = tok->str;
            node->funcname_len = tok->len;
            Node *head = NULL;
            Node *cur = NULL;

            while(!consume(")")){
                Node* expr_node = expr();
                if(!head) head = expr_node;
                else cur->next = expr_node;
                cur = expr_node;

                if (consume(")")) break;
                expect(",");
            }
            node->args = head;
                
        }else{
            node->kind = ND_LVAR;
            LVar *lvar = find_LVar(tok);
            if(lvar){
                node->offset = lvar->offset;
                node->var = lvar;
            }else{
                lvar = calloc(1, sizeof(LVar));
                lvar->str = tok->str;
                lvar->next = locals;
                lvar->len = tok->len;
                lvar->offset = locals ? locals->offset+8 : 8;
                node->offset = lvar->offset;
                node->var = lvar;
                locals = lvar;
            }
        }
        return node;

    }
    return new_node_num(expect_num());
}

Node *unary(){
    TRACE();
    if(consume("+")){
        return unary();
    }else if(consume("-")){
        return new_node(ND_SUB, new_node_num(0), unary());
    }else{
        return factor();
    }
}


Node *mul(){
    TRACE();
    Node *node = unary();

    for(;;){
        if(consume("*")){
            node = new_node(ND_MUL, node, unary());
        }
        else if(consume("/")){ node = new_node(ND_DIV, node, unary()); }else{
            return node;
        }
    }
}

Node *add(){
    TRACE();
    Node *node = mul();

    for(;;){
        if(consume("+")){
            node = new_node(ND_ADD, node, mul());
        }
        else if(consume("-")){
            node = new_node(ND_SUB, node, mul());
        }else{
            return node;
        }
    }
}


Node *relational(){
    TRACE();
    Node *node = add();

    for(;;){
        if(consume("==")){
            node = new_node(ND_EQU, node, add());
        }else if(consume("!=")){
            node = new_node(ND_NEQ, add(), node);
        }else if(consume("<=")){
            node = new_node(ND_LEQ, add(), node);
        }else if(consume(">=")){
            node = new_node(ND_LEQ, add(), node);
        }else if(consume("<")){
            node = new_node(ND_LTH, node, add());
        }else if(consume(">")){
            node = new_node(ND_LTH, add(), node);
        }else{
            return node;
        }
    }
}


Node *assign(){
    Node *node = relational();
    if(consume("="))
        node = new_node(ND_ASSIGN, node, assign());
    return node;
}


Node *equality(){
    TRACE();
    Node *node = assign();
    for(;;){
        if(consume("==")){
            node = new_node(ND_EQU, node, assign());
        }else if(consume("!=")){
            node = new_node(ND_NEQ, node, assign());
        }else{
            return node;
        }
    }
}

Node *expr(){
    TRACE();
    Node *node = equality();
    return node;
}

Node *stmt(){
    TRACE();
    Node *node;

    if(consume("{")){
        node = calloc(1, sizeof(Node));
        node->kind = ND_BLOCK;

        Node *head = NULL;
        Node *cur = NULL;
        
        while(!consume("}")){
            Node* stmt_node = stmt();
            if(!head) head = stmt_node;
            else cur->next = stmt_node;
            cur = stmt_node;
        }

        node->body = head;
        return node;
    }

    if(consume_kw(TK_IF)){
        node = calloc(1, sizeof(Node));
        node->kind = ND_IF;
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();
        if(consume_kw(TK_ELSE)){
            node->els = stmt();
        }

        return node;
    }

    if(consume_kw(TK_WHILE)){
        node = calloc(1, sizeof(Node));
        node->kind = ND_WHILE;
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();

        return node;
    }

    if(consume_kw(TK_FOR)){
        node = calloc(1, sizeof(Node));
        node->kind = ND_FOR;
        expect("(");
        if(!consume(";")){
            node->init = expr();
        }
        expect(";");
        if(!consume(";")){
            node->cond = expr();
        }
        expect(";");
        if(!consume(")")){
            node->inc = expr();
            expect(")");
        }
        node->then = stmt();

        return node;
    }

    if(consume_kw(TK_RETURN)){
        node = calloc(1, sizeof(Node));
        node->kind=ND_RETURN;
        node->lhs=expr();
    }else{
        node = expr();
    }
    expect(";");

    return node;
}

Node *function(){
}

Node *code[100];
int i = 0;
void *program(){
    while(!at_eof()){
        code[i++] = stmt();
    }
}

void dump_ast_indent(Node *node, int depth){
    if(!node) return;
    fprintf(stderr, "%*.s", depth, " ");
    if(node->kind == ND_NUM){
        fprintf(stderr, "%-8s %d\n", nd_kind_name[node->kind], node->val);
        return;
    }
    if(node->kind == ND_LVAR){
        fprintf(stderr, "%-8s %.*s\n", nd_kind_name[node->kind], node->var->len, node->var->str);
        return;
    }
    fprintf(stderr, "%-8s\n", nd_kind_name[node->kind]);
    dump_ast_indent(node->lhs, depth+1);
    dump_ast_indent(node->rhs, depth+1);
}

void dump_ast_prefix(Node *node){
    if(node->kind == ND_NUM){
        fprintf(stderr, "%d ", node->val);
        return;
    }
    fprintf(stderr, "%-7s(", nd_kind_name[node->kind]);
    dump_ast_prefix(node->lhs);
    dump_ast_prefix(node->rhs);
    fprintf(stderr, "\b)");
}



static int stack_depth=0;
static void emit_push(char *reg){
    printf("  push %s\n", reg);
    stack_depth++;
}

static void emit_push_num(int num){
    printf("  push %d\n", num);
    stack_depth++;
}

static void emit_pop(char *reg){
    printf("  pop %s\n", reg);
    stack_depth--;
}

void gen_lval(Node *node){
    if(node->kind != ND_LVAR){
        error("代入の左辺値が変数じゃないよ！");
    }
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    emit_push("rax");
}

void gen(Node *node){
    static int cnt = 0;
    static char *registers[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

    switch(node->kind){
        case ND_BLOCK: {
            Node *body = node->body;
            while(body){
                gen(body);
                emit_pop("rax");
                body = body->next;
            }
            emit_push_num(0);
            return;
        }

        case ND_RETURN:
            gen(node->lhs);
            emit_pop("rax");
            printf("  mov rsp, rbp\n");
            emit_pop("rbp");
            printf("  ret\n");
            return;
        
        case ND_IF: {
            int seq = cnt++;
            gen(node->cond);
            emit_pop("rax");
            printf("  cmp rax, 0\n");
            if(!node->els){
                printf("  je .Lend%d\n", seq);
                gen(node->then);
            }else{
                printf("  je .Lelse%d\n", seq);
                gen(node->then);
                printf("  jmp .Lend%d\n", seq);
                printf(".Lelse%d:\n", seq);
                gen(node->els);
            }
            printf(".Lend%d:\n", seq);
            emit_pop("rax");
            emit_push_num(0);
            return;
        }

        case ND_WHILE: {
            int seq = cnt++;
            printf(".Lbegin%d:\n", seq);
            gen(node->cond);
            emit_pop("rax");
            printf("  cmp rax, 0\n");
            printf("  je .Lend%d\n", seq);
            gen(node->then);
            emit_pop("rax");
            printf("  jmp .Lbegin%d\n", seq);
            printf(".Lend%d:\n", seq);
            emit_push_num(0);
            return;
        }

        case ND_FOR: {
            int seq = cnt++;
            if(node->init){
                gen(node->init);
                emit_pop("rax");
            }
            printf(".Lbegin%d:\n", seq);
            if(node->cond){
                gen(node->cond);
                emit_pop("rax");
                printf("  cmp rax, 0\n");
                printf("  je .Lend%d\n", seq);
            }
            gen(node->then);
            emit_pop("rax");
            if(node->inc){
                gen(node->inc);
                emit_pop("rax");
            }
            printf("  jmp .Lbegin%d\n", seq);
            printf(".Lend%d:\n", seq);
            emit_push_num(0);
            return;
        }

        case ND_FUNCALL: {
            int arg_cnt = 0;
            for(Node *arg=node->args; arg; arg = arg->next){
                gen(arg);
                arg_cnt++;
            }
            for(int i=arg_cnt-1; i>=0; i--){
                emit_pop(registers[i]);
            }
            printf("  call %.*s\n", node->funcname_len, node->funcname);
            emit_push("rax");
            return;
        }
    }

    switch(node->kind){
        case ND_NUM:
            emit_push_num(node->val);
            return;
        case ND_LVAR:
            gen_lval(node);
            emit_pop("rax");
            printf("  mov rax, [rax]\n");
            emit_push("rax");
            return;
        case ND_ASSIGN:
            gen_lval(node->lhs);
            gen(node->rhs);
            emit_pop("rdi");
            emit_pop("rax");
            printf("  mov [rax], rdi\n");
            emit_push("rdi");
            return;
    }

    gen(node->lhs);
    gen(node->rhs);

    emit_pop("rdi");
    emit_pop("rax");

    switch (node->kind){
        case ND_ADD:
            printf("  add rax, rdi\n");
            break;
        case ND_SUB:
            printf("  sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("  imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("  cqo\n");
            printf("  idiv rdi\n");
            break;
        case ND_EQU:
            printf("  cmp rax, rdi\n");
            printf("  sete al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_NEQ:
            printf("  cmp rax, rdi\n");
            printf("  setne al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_LEQ:
            printf("  cmp rax, rdi\n");
            printf("  setle al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_LTH:
            printf("  cmp rax, rdi\n");
            printf("  setl al\n");
            printf("  movzb rax, al\n");
            break;
        default:
            break;
    }

    emit_push("rax");
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

    emit_push("rbp");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");

    fprintf(stderr, "\n<Parse>\n");
    program();

    int j = 0;
    while(j<i){
        gen(code[j]);
        emit_pop("rax");
        fprintf(stderr, "\n<AST_%d>\n", j+1);
        dump_ast_indent(code[j], 0);
        fprintf(stderr, "\n");
        j++;
    }

    printf("  mov rsp, rbp\n");
    emit_pop("rbp");
    printf("  ret\n");
    printf(".section .note.GNU-stack,\"\",@progbits\n");
}