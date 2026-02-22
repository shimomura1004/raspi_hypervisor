ENTRY(_start)
SECTIONS
{
    . = 0x80000;
    .text : {
        *(.text.boot)
        *(.text)
    }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : {
        . = ALIGN(16);
        bss_begin = .;
        *(.bss*)
        *(COMMON)
        bss_end = .;
    }
    . = ALIGN(16);
    .stack : {
        . = . + 0x4000; /* 16KB stack */
        stack_top = .;
    }
}
