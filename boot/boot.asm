; boot/boot.asm
; Full KernelOS boot

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
    mov esp, stack_top
    push ebx    ; multiboot info
    push eax    ; magic
    call kernel_main
    cli
.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:
