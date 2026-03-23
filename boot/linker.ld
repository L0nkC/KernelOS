/* Linker script */
ENTRY(_start)

SECTIONS
{
    . = 1M;
    
    .text : ALIGN(4)
    {
        *(.text)
    }
    
    .bss : ALIGN(4)
    {
        *(.bss)
    }
}
