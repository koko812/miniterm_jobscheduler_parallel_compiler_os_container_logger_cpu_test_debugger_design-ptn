.intel_syntax noprefix
.globl main
main:
  push 2
  push 3
  push 4
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 8
  push 2
  pop rdi
  pop rax
  cqo
  idiv rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 2
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rax
  ret

.section .note.GNU-stack,"",@progbits
