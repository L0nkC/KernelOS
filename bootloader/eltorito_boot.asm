; bootloader/eltorito_boot.asm
; El Torito No-Emulation Bootloader
; Loads kernel to 0x100000 (1MB) and jumps to 0x10000C

bits 16

; Jump to start
jmp start
nop

; Boot Information Table (patched by xorriso at offset 8)
boot_info_table:
    times 8-($-$$) db 0
    dd 0                    ; LBA (patched)
    dd 0                    ; Segment (patched)  
    dd 0                    ; Offset (patched)
    times 56-($-boot_info_table) db 0

start:
    ; Save boot drive
    mov [boot_drive], dl
    
    ; Set up stack
    cli
    xor ax, ax
    mov ss, ax
    mov sp, 0x7C00
    sti
    
    ; Set up data segments
    mov ax, cs
    mov ds, ax
    mov es, ax
    
    ; Clear screen
    mov ax, 0x0003
    int 0x10
    
    mov si, msg_boot
    call print
    
    ; Load kernel to 0x10000 (64KB) first
    ; Then copy to 0x100000 using BIOS INT 15h AH=87h
    mov si, msg_load
    call print
    
    mov ah, 0x42
    mov dl, [boot_drive]
    mov si, dap
    int 0x13
    jc error
    
    mov si, msg_copy
    call print
    
    ; Copy to extended memory using INT 15h AH=87h
    mov ah, 0x87
    mov cx, 8192            ; 32K words = 64KB
    mov si, gdt_move
    int 0x15
    jc error
    
    mov si, msg_ok
    call print
    
    ; Enable A20
    in al, 0x92
    or al, 2
    out 0x92, al
    
    ; Enter protected mode
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    jmp 0x08:pm_entry

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
pm_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x120000
    jmp 0x08:0x10000C

bits 16

boot_drive: db 0
msg_boot:   db "KernelOS", 13, 10, 0
msg_load:   db "Load ", 0
msg_copy:   db "Copy ", 0
msg_ok:     db "OK", 13, 10, 0
msg_err:    db "ERR", 13, 10, 0

; DAP - Load to 0x10000 first
dap:
    db 0x10, 0
    dw 64                   ; 32KB
    dw 0x0000
    dw 0x1000               ; 0x1000:0 = 0x10000
    dq 4                    ; Start at LBA 4

; GDT for INT 15h AH=87h block move
gdt_move:
    dq 0                            ; Null
    dw 0xFFFF, 0x0000, 0x9300, 0x0000  ; Source: 0x00010000
    dw 0xFFFF, 0x0000, 0x9300, 0x009F  ; Dest: 0x00100000 (limit 0xF)

; GDT for protected mode
gdt:
    dq 0                            ; Null
    dw 0xFFFF, 0, 0x9A00, 0x00CF   ; Code: 4GB
    dw 0xFFFF, 0, 0x9200, 0x00CF   ; Data: 4GB

gdt_descriptor:
    dw gdt_descriptor - gdt - 1
    dd gdt

times 2048-($-$$) db 0
