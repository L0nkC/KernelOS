; bootloader/boot.asm
; Stage 1 Bootloader - Boot Sector for El Torito (no emulation)
; Loads stage 2 from CD and jumps to it

bits 16
org 0x7C00

; El Torito Boot Information Table
; Must be at offset 8 in the boot image
bt_info_table:
    db 0                    ; +0: Unused
    db 0                    ; +1: Unused  
    dw 0                    ; +2: Unused
    dd 0                    ; +4: Unused
    dd 0                    ; +8: Unused
    dd 0                    ; +C: Unused
    dd 0                    ; +10: LBA of boot file (filled by BIOS)
    dd 0                    ; +14: Segment of boot file (filled by BIOS)
    dw 0                    ; +18: Unused
    dw 0                    ; +1A: Unused

start:
    ; Save boot drive number (DL)
    mov [boot_drive], dl
    
    ; Set up segments and stack
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00          ; Stack grows down from boot sector
    
    ; Print boot message
    mov si, msg_boot
    call print_string
    
    ; Get disk geometry using INT 13h AH=48h (Enhanced Disk Drive)
    mov ah, 0x48
    mov dl, [boot_drive]
    mov si, ddp
    int 0x13
    jc disk_error
    
    ; Load stage 2 from CD
    ; Stage 2 is right after this boot sector in the ISO
    ; We'll load it to 0x7E00 (right after boot sector at 0x7C00)
    
    mov si, msg_load
    call print_string
    
    ; Use INT 13h AH=42h (Extended Read) to load stage 2
    ; Stage 2 is at LBA 1 (right after boot sector at LBA 0)
    mov dword [dap.lba], 1      ; Start at LBA 1
    mov word [dap.count], 64    ; Load 64 sectors (32KB) for stage 2
    mov word [dap.offset], 0x7E00
    mov word [dap.segment], 0
    
    mov ah, 0x42
    mov dl, [boot_drive]
    mov si, dap
    int 0x13
    jc disk_error
    
    mov si, msg_ok
    call print_string
    
    ; Jump to stage 2 at 0x7E00
    jmp 0x0000:0x7E00

disk_error:
    mov si, msg_disk_error
    call print_string
    jmp $

; Print string at SI
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
boot_drive:     db 0
msg_boot:       db "KernelOS Bootloader", 13, 10, 0
msg_load:       db "Loading stage 2...", 0
msg_ok:         db " OK", 13, 10, 0
msg_disk_error: db "Disk error!", 13, 10, 0

; Disk Address Packet (DAP)
align 4
dap:
    db 0x10                 ; Size of packet (16 bytes)
    db 0                    ; Reserved
.count:
    dw 0                    ; Number of blocks to transfer
.offset:
    dw 0                    ; Offset of buffer
.segment:
    dw 0                    ; Segment of buffer
.lba:
    dq 0                    ; Starting LBA

; Drive Parameter Table for INT 13h AH=48h
align 4
ddp:
    dw 0x1E                 ; Size of buffer
    dw 0                    ; Information flags
    dd 0                    ; Physical cylinders
    dd 0                    ; Physical heads
    dd 0                    ; Physical sectors per track
    dq 0                    ; Total sectors
    dw 0                    ; Bytes per sector
    dd 0                    ; EDD configuration parameters

times 2048-($-$$) db 0      ; Pad to 2048 bytes (1 CD sector)
