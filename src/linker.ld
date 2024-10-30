/* Define memory regions */
MEMORY
{
    /* UMS512 loads to 0x5500, but has a 0x200 byte header */
    RAM (rwx) : ORIGIN = 0x5700, LENGTH = 16K /* Length is not exact */
}

ENTRY(_start)

SECTIONS
{
    /* Code section, placed and executed directly from RAM */
    .text :
    {
        _stext = .;                     /* Start of text section */
        *(.text._start)                 /* Place the _start function on top */
        *(.text*)                       /* Code section */
        *(.rodata*)                     /* Read-only data */
        _etext = .;                     /* End of text section */
    } > RAM

    /* Data section for initialized data, stored and used directly from RAM */
    .data :
    {
        _sdata = .;                     /* Start of data section */
        *(.data*)                       /* Initialized data */
        _edata = .;                     /* End of data section */
    } > RAM

    /* BSS section for uninitialized data, zeroed at startup */
    .bss :
    {
        _sbss = .;                      /* Start of BSS section */
        *(.bss*)                        /* Uninitialized data */
        _ebss = .;                      /* End of BSS section */
    } > RAM

    /* Stack section */
    .stack (NOLOAD) :
    {
        _stack_start = 0x3400;          /* Start of stack */
        _stack_end = 0x4000;            /* End of stack */
    } > RAM
}
