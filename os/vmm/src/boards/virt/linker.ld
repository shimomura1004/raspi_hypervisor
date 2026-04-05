ENTRY("_start")
SECTIONS
{
	. = 0xffff000000100000;
	.text.boot : AT(0x40100000) { *(.text.boot) }
	. = ALIGN(0x00001000);
	.text : { *(.text) }
	.rodata : { *(.rodata) }
	.data : { *(.data) }
	. = ALIGN(0x8);
	bss_begin = .;
	.bss : { *(.bss*) } 
	bss_end = .;
}
