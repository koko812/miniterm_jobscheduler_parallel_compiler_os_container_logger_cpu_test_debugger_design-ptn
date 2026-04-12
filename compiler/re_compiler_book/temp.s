.intel_syntax noprefix
.globl main
main:
  mov rax, 1
  add rax, 1
  add rax, 1
  sub rax, 1
  add rax, 1
  sub rax, 1
  ret
.section .note.GNU-stack,"",@progbits
