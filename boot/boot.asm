; boot/boot.asm - With kernel_main call
section .text
global _start
extern kernel_main

; Multiboot header
align 4
    dd 0x1BADB002
    dd 0x00000003
    dd -(0x1BADB002 + 0x00000003)

_start:
    cli
    mov esp, 0x120000
    push ebx
    push eax
    call kernel_main
    cli
.hang:
    hlt
    jmp .hang
