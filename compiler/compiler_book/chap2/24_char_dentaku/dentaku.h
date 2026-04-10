#ifndef DENTAKU_H
#define DENTAKU_H

#include <stdarg.h>
#include <stdbool.h>

typedef enum{
    TY_INT,
    TY_CHAR,
    TY_PTR,
    TY_ARRAY,
} TypeKind;
typedef struct Type Type;
struct Type{
    Type *ptr_to;
    TypeKind kind;
    int array_len;
};

Type *int_type();
Type *char_type();
Type *ptr_to(Type *base);
Type *basetype();



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
    TK_CHAR,
    TK_SIZEOF,
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


typedef struct GVar GVar;
struct GVar{
    char *name;
    int len;
    Type *ty;
    GVar *next;
};
extern GVar *globals;
GVar *find_gvar(Token *tok);
void add_to_globals(Token *tok, Type *ty);


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
    ND_GVAR,
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
    GVar *gvar;
    int val;
    int offset;
    int funcname_len;
    char *funcname;
    Type *ty;
};

typedef struct LVar LVar;
struct LVar{
    LVar *next;
    LVar *next_param;
    char *name;
    int len;
    int offset;
    Type *ty;
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
    Type *ret_ty;
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
Function *function(Type *ty, Token *tok);

void add_type(Node *node, bool decay);
int size_of(Type *ty);
Type *array_of(Type *base, int len);

extern Node *code[100];
extern Function *funcs[100];
void program();
void gen(Node *node);
void gen_func(Function *fn);


#endif