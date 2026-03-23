; boot/boot.asm
; Multiboot kernel with debug exit

section .text
global _start

; Multiboot header
align 4
    dd 0x1BADB002
    dd 0x00000003
    dd -(0x1BADB002 + 0x00000003)

_start:
    cli
    mov esp, stack_top
    
    ; Clear screen to blue
    mov edi, 0xB8000
    mov ax, 0x1F20  ; blue bg, white fg, space
    mov ecx, 2000
    rep stosw
    
    ; Write "KERNEL"
    mov dword [0xB8000], 0x1F4B1F45  ; KE
    mov dword [0xB8004], 0x1F521F52  ; RR
    mov dword [0xB8008], 0x1F4E1F45  ; NE
    mov dword [0xB800C], 0x1F4C1F4F  ; OL
    mov dword [0xB8010], 0x1F211F20  ; ' '
    
    ; Success - write 0x55 to port 0x80 (POST code)
    mov al, 0x55
    out 0x80, al
    
    ; Also try isa-debug-exit
    mov al, 0x01
    out 0xf4, al
    
.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:
