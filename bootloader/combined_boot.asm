; bootloader/combined_boot.asm
; Combined boot sector + kernel loader
; This is assembled as a flat binary and loaded by El Torito
; The kernel is embedded right after this code

; Memory layout:
; 0x7C00 - 0x7E00: Boot sector (512 bytes, but we use 2048 for CD)
; 0x100000: Kernel destination

KERNEL_OFFSET equ 0x100000    ; Where kernel will be loaded
BOOT_OFFSET   equ 0x7C00      ; Where boot sector is loaded
KERNEL_SIZE   equ 64          ; Number of sectors (32KB max for now)

bits 16
org BOOT_OFFSET

; El Torito Boot Information Table (8 bytes)
btbi:
    db 0, 0, 0, 0, 0, 0, 0, 0

dw 0
dd 0
dd 0
dd 0
dd 0
dd 0
dd 0
dw 0
dw 0

start:
    jmp real_start
    nop

real_start:
    ; Save boot drive
    mov [boot_drive], dl
    
    ; Set up segments and stack
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, BOOT_OFFSET
    
    ; Clear screen
    mov ax, 0x0003
    int 0x10
    
    ; Print message
    mov si, msg_boot
    call print_string
    
    ; Check for extended disk functions
    mov ah, 0x41
    mov bx, 0x55AA
    mov dl, [boot_drive]
    int 0x13
    jc .no_ext
    cmp bx, 0xAA55
    je .has_ext
    
.no_ext:
    mov si, msg_no_ext
    call print_string
    jmp $
    
.has_ext:
    ; The kernel is embedded right after this boot code
    ; On CD, this entire image is loaded by El Torito
    ; The kernel starts at offset 2048 (sector 1 of the image)
    
    ; Copy kernel from 0x7C00 + 2048 to 0x100000
    ; For now, use a simple rep movsb approach
    
    mov si, msg_load
    call print_string
    
    ; Source: 0x7C00 + 2048 = 0x8400
    ; But we're in real mode, can't access above 1MB easily
    
    ; Alternative: Use INT 15h AH=87h for extended memory copy
    ; Or: Load in chunks to below 1MB, then copy up
    
    ; Simpler approach: Just load the kernel sectors again using INT 13h
    ; Kernel starts at LBA 1 (sector 1 of the boot image)
    
    mov dword [dap.lba], 1      ; LBA 1 (after boot sector)
    mov word [dap.count], KERNEL_SIZE
    mov word [dap.offset], 0x0000
    mov word [dap.segment], 0x1000  ; Load to 0x10000 temporarily
    
    mov ah, 0x42
    mov dl, [boot_drive]
    mov si, dap
    int 0x13
    jc disk_error
    
    mov si, msg_ok
    call print_string
    
    ; Now we have kernel at 0x10000
    ; We need to copy it to 0x100000 (1MB)
    ; Use unreal mode or BIOS INT 15h AH=87h
    
    mov si, msg_copy
    call print_string
    
    ; Enable unreal mode (big unreal mode)
    cli
    push ds
    
    ; Enable A20
    call enable_a20
    
    ; Switch to protected mode temporarily
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    jmp $+2       ; Clear pipeline
    
    ; Set data segment to flat 4GB
    mov bx, 0x08
    mov ds, bx
    mov es, bx
    
    ; Back to real mode
    and eax, ~1
    mov cr0, eax
    
    jmp $+2       ; Clear pipeline
    
    pop ds
    sti
    
    ; Now we have unlimited access with 32-bit addressing
    ; Copy kernel from 0x10000 to 0x100000
    mov esi, 0x10000
    mov edi, 0x100000
    mov ecx, KERNEL_SIZE * 512 / 4  ; DWORDs to copy
    
    a32 rep movsd   ; a32 prefix for 32-bit addressing in 16-bit mode
    
    mov si, msg_done
    call print_string
    
    ; Set up segments for kernel
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Set up stack for kernel
    mov esp, 0x120000
    
    ; Push multiboot info (dummy for now)
    push 0          ; mbi
    push 0x2BADB002 ; magic
    
    ; Jump to kernel at 0x10000c (entry point)
    jmp 0x08:0x10000c

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

enable_a20:
    ; Fast A20 enable
    in al, 0x92
    or al, 2
    out 0x92, al
    ret

; GDT
gdt_start:
    dq 0
gdt_code:
    dw 0xFFFF, 0, 0x9A00, 0x00CF
gdt_data:
    dw 0xFFFF, 0, 0x9200, 0x00CF
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; Disk Address Packet
dap:
    db 0x10, 0x00
count:  dw KERNEL_SIZE
offset: dw 0
segment: dw 0
lba:    dq 0

; Data
boot_drive:     db 0
msg_boot:       db "KernelOS", 13, 10, 0
msg_no_ext:     db "No EXT", 13, 10, 0
msg_load:       db "Load ", 0
msg_ok:         db "OK", 13, 10, 0
msg_copy:       db "Copy ", 0
msg_done:       db "Go!", 13, 10, 0
msg_disk_err:   db "ERR", 13, 10, 0

; Pad to 2048 bytes
times 2048-($-$$) db 0

; The kernel will be appended here during ISO creation
