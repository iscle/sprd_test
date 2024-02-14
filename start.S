#define FDL1_STACK 0x3000

.global _start
_start:
    b reset

reset:
    // Set stack pointer
    ldr x0, =FDL1_STACK
    mov sp, x0

    // Clear .bss section
    ldr x0, =_sbss // Start address of .bss
    ldr x1, =_ebss // End address of .bss
    mov x2, #0
clear_bss:
    cmp x0, x1     // Compare pointers to check if we've reached the end of .bss
    b.ge done_bss  // If x0 >= x1, we are done
    str x2, [x0], #8 // Store 0 (from x2) at [x0] and increment x0 by 8 bytes
    b clear_bss    // Repeat until whole .bss is cleared

done_bss:
    // Jump to main function
    b main
