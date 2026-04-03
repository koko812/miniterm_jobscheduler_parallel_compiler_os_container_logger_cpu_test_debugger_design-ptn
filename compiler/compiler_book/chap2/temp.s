.intel_syntax noprefix
.globl main
main:
  push 0
  push 2
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  push 3
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  ret

.section .note.GNU-stack,"",@progbits
