.intel_syntax noprefix
.globl main
main:
  push 3
  push 2
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 3
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  push 2
  push 10
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  ret

.section .note.GNU-stack,"",@progbits
