ENTRY(_start)
SECTIONS
{
    . = 0x5700;
    . = ALIGN(8);
    .text : {
        start.o (.text*)
        *(.text*)
    }

    . = ALIGN(8);
    .rodata : {
        *(.rodata*) /* Include read-only data */
    }

    . = ALIGN(8);
    .data : {
        *(.data*)
    }

    . = ALIGN(8);
    .bss : {
        _sbss = .; /* Start of the .bss section in RAM */
        *(.bss*) /* Include all .bss sections from your source files */
        . = ALIGN(8);
        _ebss = .; /* End of the .bss section in RAM */
    }

    /DISCARD/ : {
        *(.comment)
    }
    /DISCARD/ : { *(.dynstr*) }
    /DISCARD/ : { *(.dynamic*) }
    /DISCARD/ : { *(.plt*) }
    /DISCARD/ : { *(.interp*) }
    /DISCARD/ : { *(.gnu*) }
}
