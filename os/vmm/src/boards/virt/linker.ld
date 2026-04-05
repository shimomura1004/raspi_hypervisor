ENTRY("_start")
SECTIONS
{
	/* VMM では仮想アドレスを用いていないので LMA と VMA を分ける必要はない */
	. = 0x40100000;
	.text.boot : { *(.text.boot) }
	.text : { *(.text) }
	.rodata : { *(.rodata) }
	.data : { *(.data) }
	. = ALIGN(0x8);
	bss_begin = .;
	.bss : { *(.bss*) } 
	bss_end = .;
}
