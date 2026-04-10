#include "dentaku.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

static int labelseq = 0;
static int stack_depth = 0;
static char *argreg[] = {"rdi", "rsi", "rdx", "rcx","r8","r9"};

static bool is_ptr(Type *ty){
    return ty && ty->kind == TY_PTR;
}

static bool is_int(Type *ty){
    return ty && ty->kind == TY_INT;
}

void gen_lval(Node *node){
    if(node->kind == ND_LVAR){
        printf("  mov rax, rbp\n");
        printf("  sub rax, %d\n", node->offset);
        printf("  push rax\n");
    }else if(node->kind == ND_GVAR){
        printf("  lea rax, [rip + %.*s]\n", node->gvar->len, node->gvar->name);
        printf("  push rax\n");
    }else if(node->kind == ND_DEREF){
        gen(node->lhs);
    }else{
        error("代入の左辺値が変数ではありません．");
    }
}

void emit_push(const char *reg){
    printf("  push %s\n", reg);
    stack_depth++;
}

void emit_push_imm(int val){
    printf("  push %d\n", val);
    stack_depth++;
}

void emit_pop(const char *reg){
    printf("  pop %s\n", reg);
    stack_depth--;
}

void gen(Node *node){

    if(node->kind==ND_FUNCALL){
        int call_cnt=0;
        for(Node *n = node->args; n; n=n->next){
            gen(n);
            call_cnt++;
            if(call_cnt>=6) break;
        }
        for(int i=call_cnt-1; i>=0; i--){
            emit_pop("rax");
            printf("  mov %s, rax\n", argreg[i]);
        }
        bool need_align = (stack_depth % 2 == 0);
        if(need_align){
            printf("  sub rsp, 8\n");
            stack_depth++;
        } 
        printf("  call %.*s\n",node->funcname_len, node->funcname);
        if(need_align){
            printf("  add rsp, 8\n");
            stack_depth--;
        } 
        emit_push("rax");
        return;
    }

    if(node->kind==ND_BLOCK){
        for(Node *n = node->body; n; n=n->next){
            gen(n);
            emit_pop("rax");
        }
        emit_push_imm(0);
        return;
    }

    if(node->kind==ND_RETURN){
        gen(node->lhs);
        emit_pop("rax");
        printf("  mov rsp, rbp\n");
        emit_pop("rbp");
        printf("  ret\n");
        return;
    }

    if(node->kind==ND_IF){
        int seq = labelseq++;
        gen(node->cond);
        emit_pop("rax");
        printf("  cmp rax, 0\n");
        if(node->els){
            printf("  je  .Lelse%d\n", seq);
            gen(node->then);
            printf("  jmp  .Lend%d\n", seq);
            printf(".Lelse%d:\n", seq);
            gen(node->els);
            printf(".Lend%d:\n", seq);
        }else{
            printf("  je  .Lend%d\n", seq);
            gen(node->then);
            printf(".Lend%d:\n", seq);
        }
        emit_push_imm(0);
        return;
    }

    if(node->kind==ND_WHILE){
        int seq = labelseq++;
        printf(".Lbegin%d:\n", seq);
        gen(node->cond);
        emit_pop("rax");
        printf("  cmp rax, 0\n");
        printf("  je  .Lend%d\n", seq);
        gen(node->then);
        emit_pop("rax");
        printf("  jmp  .Lbegin%d\n", seq);
        printf(".Lend%d:\n", seq);
        emit_push_imm(0);
        return;
    }

    if(node->kind==ND_FOR){
        int seq = labelseq++;
        if(node->init){
            gen(node->init);
            emit_pop("rax");
        }
        printf(".Lbegin%d:\n", seq);
        if(node->cond){
            gen(node->cond);
            emit_pop("rax");
            printf("  cmp rax, 0\n");
            printf("  je  .Lend%d\n", seq);
        }
        gen(node->then);
        emit_pop("rax");
        if(node->inc){
            gen(node->inc);
            emit_pop("rax");
        }
        printf("  jmp  .Lbegin%d\n", seq);
        printf(".Lend%d:\n", seq);
        emit_push_imm(0);
        return;
    }

    switch(node->kind){
        case ND_NUM:
            emit_push_imm(node->val);
            return;

        case ND_LVAR:
            gen_lval(node);
            emit_pop("rax");
            if(node->ty->kind == TY_CHAR){
                printf("  movsx rax, BYTE PTR [rax]\n");
            }else{
                printf("  mov rax, [rax]\n");
            }
            emit_push("rax");
            return;

        case ND_GVAR:
            gen_lval(node);
            emit_pop("rax");
            if(node->ty->kind == TY_CHAR){
                printf("  movsx rax, BYTE PTR [rax]\n");
            }else{
                printf("  mov rax, [rax]\n");
            }
            emit_push("rax");
            return;

        case ND_ASSIGN:
            gen_lval(node->lhs);
            gen(node->rhs);
            emit_pop("rdi");
            emit_pop("rax");
            if(node->lhs->ty->kind == TY_CHAR){
                printf("  mov BYTE PTR [rax], dil\n");
            }else{
                printf("  mov [rax], rdi\n");
            }
            emit_push("rdi");
            return;

        case ND_ADDR:
            gen_lval(node->lhs);
            return;
        
        case ND_DEREF:
            gen(node->lhs);
            emit_pop("rax");
            if(node->ty->kind == TY_CHAR){
                printf("  movsx rax, BYTE PTR [rax]\n");
            }else{
                printf("  mov rax, [rax]\n");
            }
            emit_push("rax");
            return;
    }

    gen(node->lhs);
    gen(node->rhs);

    emit_pop("rdi");
    emit_pop("rax");

    if(node->kind == ND_ADD || node->kind == ND_SUB){
        if(is_ptr(node->lhs->ty) && is_int(node->rhs->ty)){
            printf("  imul rdi, %d\n", size_of(node->lhs->ty->ptr_to));
            if(node->kind == ND_ADD) printf("  add rax, rdi\n");
            else printf("  sub rax, rdi\n");
            emit_push("rax");
            return;
        }

        if(node->kind == ND_ADD && is_int(node->lhs->ty) && is_ptr(node->rhs->ty)){
            printf("  imul rax, %d\n", size_of(node->rhs->ty->ptr_to));
            printf("  add rax, rdi\n");
            emit_push("rax");
            return;
        }
    }

    switch(node->kind){
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
        case ND_LT:
            printf("  cmp rax, rdi\n");
            printf("  setl al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_LE:
            printf("  cmp rax, rdi\n");
            printf("  setle al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_EQ:
            printf("  cmp rax, rdi\n");
            printf("  sete al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_NEQ:
            printf("  cmp rax, rdi\n");
            printf("  setne al\n");
            printf("  movzb rax, al\n");
            break;
    }

    emit_push("rax");
}

void gen_func(Function *fn){
    printf(".globl %.*s\n", fn->name_len, fn->name);
    printf("%.*s:\n", fn->name_len, fn->name);
    stack_depth = 0;
    emit_push("rbp");
    printf("  mov rbp, rsp\n");

    int max_offset = 0;
    for (LVar *v = fn->locals; v; v=v->next){
        if (max_offset < v->offset) max_offset = v->offset;
    }
    printf("  sub rsp, %d\n", max_offset);
    stack_depth += max_offset / 8;

    int i = 0;
    static char *argreg[] = {"rdi", "rsi", "rdx", "rcx","r8","r9"};
    for (LVar *v = fn->params; v; v=v->next_param){
        printf("  mov [rbp-%d], %s\n", v->offset, argreg[i++]);
    }

    gen(fn->body);

    printf("  mov rsp, rbp\n");
    emit_pop("rbp");
    stack_depth = 0;
    printf("  ret\n");
}