; bootloader/final_boot.asm
; El Torito No-Emulation Bootloader

bits 16

; Jump to start (at offset 0)
jmp start
nop

; Boot info table placeholder (at offset 8)
btbi_lba:       dd 0
btbi_segment:   dd 0
btbi_unused:    dd 0

start:
    ; Copy ourselves to 0x7C00 first
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    
    ; Get current segment
    mov bx, cs
    mov ds, bx
    xor si, si
    
    ; Destination: 0x7C00
    mov ax, 0x07C0
    mov es, ax
    xor di, di
    
    ; Copy 4096 bytes
    mov cx, 2048
    rep movsw
    
    ; Jump to copied code
    jmp 0x07C0:main

main:
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti
    
    ; Clear screen
    mov ax, 0x0003
    int 0x10
    
    mov si, msg_boot
    call print
    
    ; Load kernel
    mov si, msg_load
    call print
    
    mov ah, 0x42
    xor dl, dl
    mov si, dap
    int 0x13
    jc error
    
    mov si, msg_ok
    call print
    
    ; Enable A20 and enter protected mode
    in al, 0x92
    or al, 2
    out 0x92, al
    
    cli
    lgdt [gdt_desc]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    jmp 0x08:pm_start

print:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print
.done:
    ret

error:
    mov si, msg_err
    call print
    jmp $

bits 32
pm_start:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x20000
    jmp 0x08:0x1000C

bits 16

msg_boot:  db "KernelOS", 13, 10, 0
msg_load:  db "Load...", 0
msg_ok:    db "OK", 13, 10, 0
msg_err:   db "ERR", 13, 10, 0

dap:
    db 0x10, 0, 64, 0, 0, 0, 0x10, 0
    dq 4

gdt:
    dq 0
    dw 0xFFFF, 0, 0x9A00, 0x00CF
    dw 0xFFFF, 0, 0x9200, 0x00CF

gdt_desc:
    dw gdt_desc - gdt - 1
    dd gdt

times 2048-($-$$) db 0
