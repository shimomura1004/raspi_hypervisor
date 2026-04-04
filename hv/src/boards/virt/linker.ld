ENTRY(_start)

SECTIONS
{
	. = 0x40100000;
	.text.boot : { *(.text.boot) }
	. = ALIGN(0x00001000);
	.text :  { *(.text) }
	.rodata : { *(.rodata) }
	.data : { *(.data) }
	. = ALIGN(0x8);
	bss_begin = .;
	.bss : { *(.bss*) }
	bss_end = .;
	. = ALIGN(0x00001000);
	pg_dir = .;
	.data.pgd : { . += (3 * (1 << 12)); }
}
