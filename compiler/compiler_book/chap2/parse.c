#include "dentaku.h"
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

Token *token;
char *user_input;
LVar *locals;

static bool is_ptr(Type *ty){
    return ty && ty->kind == TY_PTR;
}

static bool is_int(Type *ty){
    return ty && ty->kind == TY_INT;
}

static int align_to(int n, int align) {
    return (n+ align-1) / align * align;
}

void error_at(char *loc, char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error(char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

bool consume(char *op){
    if(token->kind != TK_RESERVED || 
        strlen(op) != token-> len ||
        memcmp(token->str, op, token->len))
        return false;
    token = token->next;
    return true;
}

bool consume_kw(TokenKind kind){
    if(token->kind != kind) return false;
    token=token->next;
    return true;
}

Token *consume_ident(){
    if(token->kind != TK_IDENT)
        return NULL;
    Token *tok = token;
    token = token->next;
    return tok;
}



void expect(char *op){
    if(token->kind != TK_RESERVED || 
        strlen(op) != token-> len ||
        memcmp(token->str, op, token->len))
        error_at(token->str, "'%s'ではありません.", op);
    int val = token->val;
    token = token->next;
}

void expect_kw(TokenKind kind){
    if(token->kind != kind)
        error_at(token->str, "キーワードが必要です．");
    token = token->next;
}

int expect_number(){
    if(token->kind != TK_NUM)
        error_at(token->str, "数ではありません．");
    int val = token->val;
    token = token->next;
    return val;
}


Token *expect_ident(){
    if(token->kind != TK_IDENT)
        error_at(token->str, "識別子が必要です.");
    Token *tok = token;
    token = token->next;
    return tok;
}

bool at_eof(){
    return token->kind == TK_EOF;
}



Token *new_token(TokenKind kind, Token *cur, int len, char *str){
    Token *tok = calloc(1, sizeof(Token));

    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

int is_alnum(char c){
    return ('a' <= c && c <= 'z') ||
        ('A' <= c && c <= 'Z') ||
        ('0' <= c && c <= '9') ||
        (c == '_');
}

Token *tokenize(){
    Token head;
    head.next = NULL;
    Token *cur = &head;
    char *p = user_input;

    while(*p){
        if(isspace(*p)){
            p++;
            continue;
        }

        if(strncmp(p, "sizeof", 6) == 0 && !is_alnum(p[6])){
            cur = new_token(TK_SIZEOF, cur, 6, p);
            p+=6;
            continue;
        }


        if(strncmp(p, "int", 3) == 0 && !is_alnum(p[3])){
            cur = new_token(TK_INT, cur, 3, p);
            p+=3;
            continue;
        }

        if(strncmp(p, "while", 5) == 0 && !is_alnum(p[5])){
            cur = new_token(TK_WHILE, cur, 5, p);
            p+=5;
            continue;
        }

        if(strncmp(p, "for", 3) == 0 && !is_alnum(p[3])){
            cur = new_token(TK_FOR, cur, 3, p);
            p+=3;
            continue;
        }

        if(strncmp(p, "if", 2) == 0 && !is_alnum(p[2])){
            cur = new_token(TK_IF, cur, 2, p);
            p+=2;
            continue;
        }

        if(strncmp(p, "else", 4) == 0 && !is_alnum(p[4])){
            cur = new_token(TK_ELSE, cur, 4, p);
            p+=4;
            continue;
        }

        if(strncmp(p, "return", 6) == 0 && !is_alnum(p[6])){
            cur = new_token(TK_RETURN, cur, 6, p);
            p+=6;
            continue;
        }

        if (isalpha(*p) || *p == '_'){
            char *start = p;
            p++;
            while (is_alnum(*p) || *p == '_'){
                p++;
            }
            int len = p - start;
            cur = new_token(TK_IDENT, cur, len, start);
            continue;
        }

        if (!strncmp(p, "==", 2) ||
            !strncmp(p, "!=", 2) ||
            !strncmp(p, "<=", 2) ||
            !strncmp(p, ">=", 2)) {
                cur = new_token(TK_RESERVED, cur, 2, p);
                p += 2;
                continue;
            }

        if (strchr("+-*/()<>=;{},&[]", *p)){
            cur = new_token(TK_RESERVED, cur, 1, p++);
            continue;
        }

        if(isdigit(*p)){
            cur = new_token(TK_NUM, cur, 0, p);
            cur->val = strtol(p, &p, 10);
            cur->len = p - cur->str;
            continue;
        }

        error_at(p, "トークナイズできません．");
    }

    new_token(TK_EOF, cur, 0, p);
    token = head.next;
    return head.next;
}


Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
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


Node *primary(){
    Node *node;

    if(consume("(")){
        Node *node = expr();
        expect(")");
        return node;
    }


    Token *tok = consume_ident();
    if(tok){
        node = calloc(1, sizeof(Node));
        if(consume("(")){
            node->kind = ND_FUNCALL;
            node->funcname = tok->str;
            node->funcname_len = tok->len;
            Node *head = NULL;
            Node *cur = NULL;

            if(!consume(")")){
                for (;;){
                    Node *expr_node = expr();
                    if(!head) head = expr_node;
                    else cur->next = expr_node;
                    cur = expr_node;

                    if (consume(")")) break;
                    expect(",");
                }
            }

            node->args = head;
            return node;

        }else{
            node->kind = ND_LVAR;
            LVar *lvar = find_lvar(tok);
            if(!lvar) error_at(tok->str, "未定義の変数です．");
            node->offset = lvar->offset;
            node->ty = lvar->ty;
            return node;
        }
    }

    while (consume("[")){
        Node *idx = expr();
        expect("]");
        node = new_node(ND_DEREF, new_node(ND_ADD, node, idx), NULL);
    }

    return new_node_num(expect_number());
}

Node *unary(){
    if(consume_kw(TK_SIZEOF)){
        Node *node = unary();
        add_type(node, false);
        return new_node_num(size_of(node->ty));
    }
    if (consume("+"))
        return primary();
    if (consume("-"))
        return new_node(ND_SUB, new_node_num(0), primary());
    if (consume("*"))
        return new_node(ND_DEREF, unary(), NULL);
    if (consume("&"))
        return new_node(ND_ADDR, unary(), NULL);

    return primary();
}

Node *mul(){
    Node *node = unary();

    for(;;){
        if(consume("*"))
            node = new_node(ND_MUL, node, unary());
        else if(consume("/"))
            node = new_node(ND_DIV, node, unary());
        else{
            return node;
        }
    }
}

Node *add(){
    Node *node = mul();
    for(;;){
        if(consume("+"))
            node = new_node(ND_ADD, node, mul());
        else if(consume("-"))
            node = new_node(ND_SUB, node, mul());
        else{
            return node;
        }
    }
}

Node *relational(){
    Node *node = add();
    for(;;){
        if(consume("<"))
            node = new_node(ND_LT, node, add());
        else if(consume(">"))
            node = new_node(ND_LT, add(), node);
        else if(consume("<="))
            node = new_node(ND_LE, node, add());
        else if(consume(">="))
            node = new_node(ND_LE, add(), node);
        else{
            return node;
        }
    }
}

Node *equality(){
    Node *node = relational();
    for(;;){
        if(consume("=="))
            node = new_node(ND_EQ, node, relational());
        else if(consume("!="))
            node = new_node(ND_NEQ, node, relational());
        else{
            return node;
        }
    }
}

Node *assign(){
    Node *node = equality();
    if(consume("=")){
        node = new_node(ND_ASSIGN, node, assign());
    }
    return node;
}


Node *expr(){
    Node *node = assign();
    return node;
}

Node *stmt(){
    Node *node;

    if(consume_kw(TK_INT)){
        Type *ty = basetype();
        Token *t = expect_ident();

        if(consume("[")) {
            int len = expect_number();
            expect("]");
            ty = array_of(ty, len);
        }

        LVar *lvar = calloc(1, sizeof(LVar));
        lvar->name = t->str;
        lvar->len = t->len;
        int size = align_to(size_of(ty), 8);
        lvar->ty = ty;
        lvar->offset = locals ? locals->offset + size : size;
        lvar->next = locals;
        locals = lvar;

        expect(";");
        return new_node_num(0);
    }

    if(consume("{")){
        node = calloc(1, sizeof(Node));
        node->kind = ND_BLOCK;
        Node *head = NULL;
        Node *cur = NULL;
        while(!consume("}")){
            Node *stmt_node = stmt();
            if(!head){
                head = stmt_node;
            }else{
                cur->next = stmt_node;
            }
            cur = stmt_node;
        }

        node->body = head;
        return node;
    }

    if(consume_kw(TK_RETURN)){
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr();
        expect(";");
        return node;
    }

    if(consume_kw(TK_IF)){
        node = calloc(1, sizeof(Node));
        node->kind = ND_IF;
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();
        if (consume_kw(TK_ELSE))
            node->els = stmt();
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
            expect(";");
        }
        if(!consume(";")){
            node->cond = expr();
            expect(";");
        }
        if(!consume(")")){
            node->inc = expr();
            expect(")");
        }

        node->then = stmt();
        return node;
    }

    node = expr();
    expect(";");
    return node;
}

Function *function(){
    expect_kw(TK_INT);
    Type *ty = basetype();
    Token *tok = expect_ident();

    Function *fn = calloc(1, sizeof(Function));
    fn->name = tok->str;
    fn->name_len = tok->len;
    fn->ret_ty = ty;
    locals = NULL;
    fn->params = NULL;

    LVar *param_head = NULL;
    LVar *param_cur = NULL;

    expect("(");
    if(!consume(")")){
        for (;;){
            expect_kw(TK_INT);
            Type *ty = basetype();
            Token *t = expect_ident();

            LVar *lvar = calloc(1, sizeof(LVar));
            lvar->name = t->str;
            lvar->len = t->len;
            int size = align_to(size_of(ty), 8);
            lvar->ty = ty;
            lvar->offset = locals ? locals->offset + size : size;

            lvar->next = locals;
            locals = lvar;

            if (!param_head) param_head = lvar;
            else param_cur->next_param = lvar;
            param_cur = lvar;

            if (consume(")")) break;
            expect(",");
        }
    }

    fn->params = param_head;
    fn->body = stmt();
    fn->locals = locals;
    return fn;
}

Function *funcs[100];
void program(){
    int i = 0;
    while(!at_eof()){
        funcs[i++] = function();
    }
    funcs[i]=NULL;
}


LVar *find_lvar(Token *tok){
    for(LVar *var = locals; var; var = var->next){
        if(var->len==tok->len && !memcmp(tok->str, var->name, var->len)){
            return var;
        }
    }
    return NULL;
}

Type *basetype(){
    Type *ty = int_type();
    while(consume("*")){
        ty = ptr_to(ty);
    }
    return ty;
}

Type *int_type(){
    Type *ty = calloc(1, sizeof(Type));
    ty->kind = TY_INT;
    return ty;
}

Type *ptr_to(Type *base){
    Type *ty = calloc(1, sizeof(Type));
    ty->kind = TY_PTR;
    ty->ptr_to = base;
    return ty;
}

int size_of(Type *ty){
    switch(ty->kind){
        case TY_INT: return 4;
        case TY_PTR: return 8;
        case TY_ARRAY: return size_of(ty->ptr_to) * ty->array_len;
        // sizeof の挙動がなんかいまいちわからん．
    }
    return 0;
}

Type *array_of(Type *base, int len){
    Type *ty = calloc(1, sizeof(Type));
    ty->kind = TY_ARRAY;
    ty->ptr_to = base;
    ty->array_len = len;
    return ty;
}

void add_type(Node *node, bool decay){
    if(!node) return;
    if(node->ty && node->kind != ND_LVAR) return;

    switch(node->kind){
        case ND_NUM:
            node->ty = int_type();
            return;
        case ND_ADD:
            add_type(node->lhs, true);
            add_type(node->rhs, true);
            if(is_ptr(node->lhs->ty) && is_int(node->rhs->ty))
                node->ty = node->lhs->ty;
            else if(is_int(node->lhs->ty) && is_ptr(node->rhs->ty))
                node->ty = node->rhs->ty;
            else
                node->ty = int_type();
            return;
        case ND_SUB:
            add_type(node->lhs, true);
            add_type(node->rhs, true);
            if(is_ptr(node->lhs->ty) && is_int(node->rhs->ty))
                node->ty = node->lhs->ty;
            else
                node->ty = int_type();
            return;
        case ND_LVAR:
            if (decay && node->ty->kind == TY_ARRAY){
                node->ty = ptr_to(node->ty->ptr_to);
            }
            return;
        case ND_ASSIGN:
            add_type(node->lhs, true);
            add_type(node->rhs, true);
            node->ty = node->lhs->ty;
            return;
        case ND_ADDR:
            add_type(node->lhs, false);
            node->ty = ptr_to(node->lhs->ty);
            return;
        case ND_DEREF:
            add_type(node->lhs, true);
            if(node->lhs->ty->kind == TY_PTR){
                node->ty = node->lhs->ty->ptr_to;
            }else{
                fprintf(stderr, "kind=%d\n", node->lhs->ty->kind);
                error("間接参照の型がポインタではありません．");
            }
            return;
        case ND_FUNCALL:
            for(Node *n= node->args; n; n=n->next)
                add_type(n, true);
            node->ty = int_type();
            return;
        case ND_BLOCK:
            for(Node *n= node->body; n; n=n->next)
                add_type(n, true);
            node->ty = int_type();
            return;
        default:
            add_type(node->lhs, true);
            add_type(node->rhs, true);
            node->ty = node->lhs ? node->lhs->ty : int_type();
            return;

    }
}