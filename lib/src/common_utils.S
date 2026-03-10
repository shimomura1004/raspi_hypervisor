.globl put32
put32:
	str w1,[x0]
	ret

.globl get32
get32:
	ldr w0,[x0]
	ret

.globl get_el
get_el:
	mrs x0, CurrentEL
	lsr x0, x0, #2
	ret

.globl get_cpuid
get_cpuid:
	mrs x0, mpidr_el1
	and x0, x0, #0xff
	ret

// スタックが有効になっていない起動直後のタイミングで使えるアセンブリ実装の memzero
// コピー元とコピー先の領域が重複していると誤作動する
.globl memzero_without_stack
memzero_without_stack:
	mov x2, x0           // 戻り値(dst)を退避
	cbz x1, 3f           // サイズが0なら終了
1:
	cmp x1, #8
	b.lo 2f              // 残りが8バイト未満なら1バイト処理へ
	str xzr, [x0], #8    // 8バイトゼロクリア (post-index)
	sub x1, x1, #8
	b 1b
2:
	cbz x1, 3f           // 残りが0なら終了
	strb wzr, [x0], #1   // 1バイトゼロクリア
	sub x1, x1, #1
	b 2b
3:
	mov x0, x2           // 戻り値を設定
	ret

.globl delay
delay:
	subs x0, x0, #1
	bne delay
	ret
