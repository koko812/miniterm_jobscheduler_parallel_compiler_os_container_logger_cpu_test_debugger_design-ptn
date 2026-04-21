.intel_syntax noprefix
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
  push 3
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  push 8
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
