.intel_syntax noprefix
.globl main
main:
  mov rax, 123
  sub rax, 23
  add rax, 4
  add rax, 45
  sub rax, 56
  ret

.section .note.GNU-stack,"",@progbits
