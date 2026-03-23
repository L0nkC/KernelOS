; boot/boot.asm
; Multiboot header MUST be first in the file

section .multiboot_header
align 4
    dd 0x1BADB002       ; magic
    dd 0x00000003       ; flags (ALIGN | MEMINFO)
    dd -(0x1BADB002 + 0x00000003)  ; checksum

section .text
bits 32
global _start
extern kernel_main

_start:
    ; Stack setup
    mov esp, stack_top
    
    ; Save multiboot info
    push ebx
    push eax
    
    ; Call kernel
    call kernel_main
    
    ; Halt forever
    cli
.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384          ; 16KB stack
stack_top:
