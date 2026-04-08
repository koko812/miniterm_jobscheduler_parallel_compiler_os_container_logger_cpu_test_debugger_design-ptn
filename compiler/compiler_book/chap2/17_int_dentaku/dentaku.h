#ifndef DENTAKU_H
#define DENTAKU_H

#include <stdarg.h>
#include <stdbool.h>

typedef enum{
    TK_RESERVED,
    TK_IDENT,
    TK_RETURN,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_NUM,
    TK_INT,
    TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token{
    TokenKind kind;
    Token *next;
    int val;
    char *str;
    int len;
};

extern Token *token;
extern char *user_input;

typedef enum{
    ND_EQ,
    ND_NEQ,
    ND_LE,
    ND_LT,
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_ASSIGN,
    ND_RETURN,
    ND_IF,
    ND_WHILE,
    ND_FOR,
    ND_LVAR,
    ND_NUM,
    ND_ADDR,
    ND_DEREF,
    ND_BLOCK,
    ND_FUNCALL,
} NodeKind;

typedef struct Node Node;

struct Node{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    Node *cond;
    Node *then;
    Node *els;
    Node *init;
    Node *inc;
    Node *next;
    Node *body;
    Node *args;
    int val;
    int offset;
    int funcname_len;
    char *funcname;
};

typedef struct LVar LVar;

struct LVar{
    LVar *next;
    LVar *next_param;
    char *name;
    int len;
    int offset;
};

extern LVar *locals;
LVar *find_lvar(Token *tok);


typedef struct Function Function;

struct Function{
    char *name;
    int name_len;
    Node *body;
    LVar *locals;
    LVar *params;
};

void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);

bool consume(char *op);
Token *consume_ident();
bool consume_kw(TokenKind kind);

void expect(char *op);
void expect_kw(TokenKind kind);
int expect_number();
Token *expect_ident();
bool at_eof();


Token *new_token(TokenKind kind, Token *cur, int len, char *str);
Token *tokenize();
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);

Node *expr();
Node *primary();
Node *unary();
Node *mul();
Node *add();
Node *relational();
Node *equality();

extern Node *code[100];
extern Function *funcs[100];
void program();
void gen(Node *node);
void gen_func(Function *fn);

#endif