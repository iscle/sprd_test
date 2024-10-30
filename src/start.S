.section .text._start

_start:
    /* Set up the stack pointer */
    ldr x0, =_stack_end /* Load the address of the end of the stack */
    mov sp, x0          /* Set the stack pointer */

    /* Zero initialize the .bss section */
    ldr x0, =_sbss      /* Start of .bss in RAM */
    ldr x1, =_ebss      /* End of .bss in RAM */
zero_bss:
    cmp x0, x1          /* Check if end of .bss is reached */
    b.ge end_zero_bss   /* If we've reached the end, skip zeroing */
    str xzr, [x0], #8   /* Store zero (XZR) and increment pointer by 8 bytes */
    b zero_bss          /* Repeat until entire .bss is zeroed */

end_zero_bss:
    /* Jump to main */
    b main              /* Branch to main function */
