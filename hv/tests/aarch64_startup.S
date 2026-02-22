.section ".text.boot"
.globl _start
_start:
    mrs x0, mpidr_el1
    and x0, x0, #0xFF
    cbz x0, master
hang:
    wfe
    b hang

master:
    // Setup stack
    ldr x0, =stack_top
    mov sp, x0

    // Zero BSS
    ldr x0, =bss_begin
    ldr x1, =bss_end
    sub x1, x1, x0
    cbz x1, jump_main
zero_loop:
    str xzr, [x0], #8
    subs x1, x1, #8
    b.gt zero_loop

jump_main:
    bl aarch64_main
    b hang
