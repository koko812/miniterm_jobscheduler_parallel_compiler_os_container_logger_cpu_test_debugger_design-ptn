.intel_syntax noprefix
.globl main
main:
  push 123
  push 23
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  push 4
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 45
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 56
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rax
  ret

.section .note.GNU-stack,"",@progbits
