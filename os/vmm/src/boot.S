#include "arm/sysregs_def.h"
#include "mm.h"

.section ".text.boot"

.globl _start
_start:
    mrs x0, mpidr_el1
    and x0, x0,#0xFF    // Check processor id
    cbz x0, el3_entry   // Hang for all non-primary CPU
    b   proc_hang

proc_hang:
    b   proc_hang

el3_entry:
    bl get_el
    cmp x0, #1
    b.eq el1_entry

    ldr x0, =SCTLR_EL2_VALUE_MMU_DISABLED
    msr sctlr_el2, x0

    ldr x0, =HCR_EL2_VALUE_FOR_OS
    msr hcr_el2, x0

    ldr x0, =SCR_EL3_VALUE_FOR_OS
    msr scr_el3, x0

    ldr x0, =SPSR_EL3_VALUE_FOR_OS
    msr spsr_el3, x0

    adr x0, el1_entry
    msr elr_el3, x0

    eret

el1_entry:
    adr x0, bss_begin
    adr x1, bss_end
    sub x1, x1, x0
    bl  memzero_without_stack

    ldr x0, =stack_top
    mov sp, x0
    bl  kernel_main
    b   proc_hang       // should never come here
