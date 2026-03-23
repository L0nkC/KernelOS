; kernel/isr.asm
; ISR and IRQ handlers

section .text

global isr0, isr13, isr14
global irq0, irq1, irq12

extern isr_handler, irq_handler

; Exceptions
isr0:
    push 0
    push 0
    jmp isr_common

isr13:
    push 0
    push 13
    jmp isr_common

isr14:
    push 14
    jmp isr_common

isr_common:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    push esp
    call isr_handler
    add esp, 4
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret

; IRQs
irq0:
    pusha
    push 0
    call irq_handler
    add esp, 4
    popa
    iret

irq1:
    pusha
    push 1
    call irq_handler
    add esp, 4
    popa
    iret

irq12:
    pusha
    push 12
    call irq_handler
    add esp, 4
    popa
    iret
