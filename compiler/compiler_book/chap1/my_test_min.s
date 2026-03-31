.intel_syntax noprefix
.global _start

_start:
    mov eax, 3
    add eax, 5
    imul eax, 2

    mov edi, eax
    mov eax, 60
    syscall
