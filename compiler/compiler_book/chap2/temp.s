.intel_syntax noprefix
.globl main
main:
  push 3
  push 3
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  ret

.section .note.GNU-stack,"",@progbits
