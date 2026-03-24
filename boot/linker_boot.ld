/* Linker script for custom bootloader */
ENTRY(_start)

SECTIONS
{
    . = 0x10000;
    
    .text : ALIGN(4)
    {
        *(.text)
    }
    
    .bss : ALIGN(4)
    {
        *(.bss)
    }
}
