#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// この辺の typedef あたりも考察を深めたい
// この設計意図について考えたい．
typedef enum{
    TK_RESERVED,
    TK_NUM,
    TK_EOF,
} TokenKind;

// typedef の仕組みもよくわかってない，なぜ token が2回必要？
typedef struct Token Token;

// 上の typedef は , でこっちは ; なのもよくわからん
struct Token{
    TokenKind kind;
    Token *next;
    int val;
    char *str;
};

// これがグローバル変数なせいか，後ろがちょっとわかりづらい？
// まあ複雑になるので嫌だよね．
Token *token;

void error(char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// ここで多分，次に進めるということをやってる．
bool consume(char op){
    if(token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    token = token->next;
    return true;
}
// トークナイザを入れるだけでこんなに複雑に．
// 一回ナイーブに作ってみて分解する過程もできれば入れてみたいものだ．
// なんかゲーム作ってる時の感覚と似てるので，いずれは自作言語でゲームも作ろうかな．

// 上の consume との違いがいまいちわからん
// 数字かオペランド（演算子）かの違いかと思ってたが，なんかそうでもなさそう
// というか，その二つを期待してるのか，入力の順番を見てるのかな．
// 上の consume はスペースとかを省いている感じなのかね．
void expect(char op){
    if(token->kind != TK_RESERVED || token->str[0] != op)
        error("'%c'ではありません.", op);
    int val = token->val;
    token = token->next;
    // そもそも，セミコロンで区切る実装はいつ入れられるんだろう
    // ファイル単位で入力する何かを実装する時に多分そういうのが入ってくるのかね
    // まだやったことないので楽しみではある．
}

// なぜわざわざ数字バージョンも分けて作るのか
// そしてトークンを進めていいのか
int expect_number(){
    if(token->kind != TK_NUM)
        error("数ではありません．");
    int val = token->val;
    token = token->next;
    return val;
}

// ここの bool がインテリセンスにはじかれん仕組みがようわからんな
// 膨大な stdclib を読みに行ってるのかという話？
bool at_eof(){
    return token->kind == TK_EOF;
}

// 写経してるし，なんとなくコードの意味はわかってきたが，
// これを自分で一から書けるかというと全くそうではない．
// そのため，C で別種類のプログラム + 別言語で tokenize & parse の
// プログラムを書いて練習してみたいという話．
// まあともかく，calloc とか使ってるのはかっこいい
// あと，構造体の手前の引数で，自己定義の struct を使うのもカッコ良い
// これは python でもできるはず？（いやないか,戻り値の定義は普通かかんもんね）

Token *new_token(TokenKind kind, Token *cur, char *str){
    Token *tok = calloc(1, sizeof(Token));
    // これ，実体じゃなくて，型のサイズでいいのか？まあいいか
    // 構造体扱うときは，こんな感じで alloc 系を使うのかね．
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
    // うーんマジで，ここの関数の存在理由がわからん．
    // あと，calloc のあとは close しなくていいのか？
    // まあここで close するのは意味わからんきもするが．
}

Token *tokenize(char *p){
    // ↑ のもそうなんだが，ポインタ型の関数ってのがよくわからん
    // これも構造体を扱ってるからか？そもそもポインタ型の定義は，引数なのか戻り値なのか．
    // まあ多分戻り値だよね？？いやそれは Token の部分だけか？
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while(*p){
        // なんか空白スキップしてるけど，これは上の方の処理で入ってなかったのか？
        // というか，なんか nextToken とか作ってたのに，使わないの？という感じ．
        if(isspace(*p)){
            p++;
            continue;
        }

        if(*p=='+' || *p=='-'){
            // 多分，演算子は 1文字確定で，digit はそうじゃないから，
            // ここでは，直接ポインタをインクリメントしている．
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if(isdigit(*p)){
            cur = new_token(TK_NUM, cur, p);
            // この 10進法とかいう指定，なんか競プロに使えそうだな？
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error("トークナイズできません．");
    }

    // うーんさっきからなんか，この辺の p とかの扱いがよくわからん．
    // あと，strtol の使い道もよくわかってない
    new_token(TK_EOF, cur, p);
    return head.next;
}


int main(int argc, char **argv){
    if (argc != 2){
        fprintf(stderr, "引数の数が正しくありません");
        return 1;
    }

    token = tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    printf("  mov rax, %d\n", expect_number());

    while(!at_eof()){
        if(consume('+')){
            printf("  add rax, %d\n", expect_number());
            continue;
        }

        expect('-');
        printf("  sub rax, %d\n", expect_number());
    }

    printf("  ret\n\n");
    printf(".section .note.GNU-stack,\"\",@progbits\n");

    return 0;
}