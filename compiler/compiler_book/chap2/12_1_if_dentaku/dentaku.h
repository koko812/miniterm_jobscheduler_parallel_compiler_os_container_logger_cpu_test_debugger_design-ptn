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
    TK_NUM,
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
    ND_LVAR,
    ND_NUM,
} NodeKind;

typedef struct Node Node;

struct Node{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    Node *cond;
    Node *then;
    Node *els;
    int val;
    int offset;
};

typedef struct LVar LVar;

struct LVar{
    LVar *next;
    char *name;
    int len;
    int offset;
};

extern LVar *locals;
LVar *find_lvar(Token *tok);

void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);

bool consume(char *op);
Token *consume_ident();
bool consume_kw(TokenKind kind);

void expect(char *op);
int expect_number();
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
void program();
void gen(Node *node);

#endif