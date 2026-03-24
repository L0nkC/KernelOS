/* Linker script */
ENTRY(_start)

SECTIONS
{
    . = 0x100000;
    
    .text : ALIGN(4)
    {
        *(.text)
    }
    
    .bss : ALIGN(4)
    {
        *(.bss)
    }
}
