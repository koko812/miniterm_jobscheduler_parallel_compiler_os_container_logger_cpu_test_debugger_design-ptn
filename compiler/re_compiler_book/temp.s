.intel_syntax noprefix
.globl main
main:
  push 4
  push 3
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 4
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  ret
.section .note.GNU-stack,"",@progbits
