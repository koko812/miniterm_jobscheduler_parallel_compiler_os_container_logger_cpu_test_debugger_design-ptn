.intel_syntax noprefix
.globl set5
set5:
  push rbp
  mov rbp, rsp
  sub rsp, 208
  mov [rbp-8], rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 5
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  pop rax
  push 0
  mov rsp, rbp
  pop rbp
  ret
.globl main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 208
  push 0
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  push 1
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rdi
  call set5
  push rax
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  pop rax
  push 0
  mov rsp, rbp
  pop rbp
  ret
.section .note.GNU-stack,"",@progbits
