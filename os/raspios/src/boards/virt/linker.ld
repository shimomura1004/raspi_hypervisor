ENTRY("_start")
SECTIONS
{
	/* qemu virt 環境では物理アドレスの先頭1MB(0x40000000-0x40100000)に必ず DTB が置かれる
	   よって恒等マッピングを行っている今の実装では、
	   仮想アドレス 0xffff000000100000 にカーネルがロードされるようにするのが適切
	   MMU が有効になるとカーネルは TTBR1_EL1 のほうを使うので、0xffff... にマップしなければいけない
	 */
	. = 0xffff000000100000;
	.text.boot : { *(.text.boot) }
	. = ALIGN(0x00001000);
	user_begin = .;
	.text.user : { build/user* (.text) }
	.rodata.user : { build/user* (.rodata) }
	.data.user : { build/user* (.data) }
	.bss.user : { build/user* (.bss) }
	user_end = .;
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

