; bootloader/simple_boot.asm
; Simple El Torito boot loader - loads flat binary kernel
; Assembled as a no-emulation boot image

bits 16
org 0x7C00

start:
    ; Save boot drive
    mov [boot_drive], dl
    
    ; Set up segments
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    
    ; Clear screen
    mov ax, 0x0003
    int 0x10
    
    ; Print boot message
    mov si, msg_boot
    call print_string
    
    ; Check for CD-ROM extensions
    mov ah, 0x41
    mov bx, 0x55AA
    mov dl, [boot_drive]
    int 0x13
    jc no_cd_extensions
    cmp bx, 0xAA55
    jne no_cd_extensions
    
    ; Load kernel using INT 13h AH=42h
    ; Kernel is at LBA 16 (8KB after boot sector)
    mov si, msg_load
    call print_string
    
    ; Load to 0x100000 (1MB) - using unreal mode or multiple loads
    ; First load to below 1MB, then copy up
    
    ; Load 64 sectors (32KB) to 0x8000 first
    mov dword [dap.lba], 16     ; Start at LBA 16
    mov word [dap.count], 64    ; 32KB
    mov word [dap.offset], 0x8000
    mov word [dap.segment], 0
    
    mov ah, 0x42
    mov dl, [boot_drive]
    mov si, dap
    int 0x13
    jc disk_error
    
    mov si, msg_ok
    call print_string
    
    ; Now copy from 0x8000 to 0x100000 using unreal mode
    ; Or use BIOS INT 15h AH=87h (block move)
    
    mov si, msg_copy
    call print_string
    
    ; Use BIOS INT 15h AH=87h to copy to extended memory
    mov ah, 0x87
    mov cx, 64                  ; 64 words = 128 bytes per iteration
    mov si, gdt_block_move
    int 0x15
    jc disk_error
    
    ; Actually, let's just load the kernel to 0x10000 (64KB) first
    ; and use a simple approach
    
    ; For simplicity in this version:
    ; Load kernel to 0x10000 and jump there
    ; The kernel needs to be compiled to run at that address
    
    mov dword [dap.lba], 16
    mov word [dap.count], 128   ; 64KB
    mov word [dap.offset], 0x0000
    mov word [dap.segment], 0x1000  ; 0x1000:0x0000 = 0x10000
    
    mov ah, 0x42
    mov dl, [boot_drive]
    mov si, dap
    int 0x13
    jc disk_error
    
    mov si, msg_jump
    call print_string
    
    ; Wait for key
    ;xor ah, ah
    ;int 0x16
    
    ; Jump to kernel at 0x10000
    jmp 0x1000:0x0000

no_cd_extensions:
    mov si, msg_no_ext
    call print_string
    jmp $

disk_error:
    mov si, msg_disk_err
    call print_string
    jmp $

print_string:
    pusha
.loop:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0E
    mov bh, 0
    mov bl, 0x07
    int 0x10
    jmp .loop
.done:
    popa
    ret

; Data
boot_drive:      db 0
msg_boot:        db "KernelOS", 13, 10, 0
msg_load:        db "Load...", 0
msg_ok:          db "OK", 13, 10, 0
msg_copy:        db "Copy...", 0
msg_jump:        db "Go!", 13, 10, 0
msg_no_ext:      db "No CD ext", 13, 10, 0
msg_disk_err:    db "Disk err", 13, 10, 0

; Disk Address Packet
dap:
    db 0x10                 ; Size
    db 0                    ; Reserved
.count:
    dw 0                    ; Count
.offset:
    dw 0                    ; Offset
.segment:
    dw 0                    ; Segment
.lba:
    dq 0                    ; LBA

; GDT for INT 15h AH=87h (not used in simplified version)
gdt_block_move:
    dw 0, 0, 0, 0           ; Null
    dw 0xFFFF, 0, 0x9300, 0x00CF  ; Source (0x00000000)
    dw 0xFFFF, 0, 0x9300, 0x00CF  ; Dest   (0x00100000)
    dw 0, 0, 0, 0           ; Null

; El Torito Boot Information Table
times 8-($-$$) db 0
dw 0
dd 0
dd 0
dd 0
dd 0
dd 0
dd 0
dw 0
dw 0

; Pad to 2048 bytes (1 CD sector)
times 2048-($-$$) db 0
