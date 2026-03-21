#include "arm/mmu.h"
#include "arm/sysregs.h"
#include "mm.h"
#include "peripherals/base.h"

.section ".text.boot"

.extern initialized

// todo: 動作確認のため、ハイパーバイザ有無に関わらず同一ソースで起動できるようにしたい
// qemu を -M raspi3b で起動すると EL2 から起動されるため、ネイティブとゲストのソースに差がでてしまう
// 起動後に EL を見て処理を切り替えれば暫定対応できる

//======== create_pgd_pud_entry ===============================================
.macro	create_pgd_pud_entry, tbl, virt, tmp1, tmp2
	// for PGD (PGD は1エントリで512GBをカバーする)
	add	\tmp2, \tbl, #PAGE_SIZE				// PUD のアドレスを計算 (PUD は PGD のすぐ後ろ)
	create_table_entry \tbl, \virt, PGD_SHIFT, \tmp2, \tmp1	// create PGD entry
	mov	\tbl, \tmp2					// PUD のアドレスをセット

	// for PUD (PUD は1エントリで1GBをカバーする)
	add	\tmp2, \tbl, #PAGE_SIZE				// PMD のアドレスを計算
	create_table_entry \tbl, \virt, PUD_SHIFT, \tmp2, \tmp1	// create PUD entry
	mov	\tbl, \tmp2					// PMD のアドレスをセット
.endm

//======== create_table_entry ===============================================
// PUD, PMD のエントリを、上位のテーブルに追記するマクロ
.macro	create_table_entry, tbl, virt, shift, next_tbl, tmp1
	// 割当てる仮想アドレスを右シフトしてインデックス部分を一番右に寄せる
	lsr	\tmp1, \virt, #\shift
	// ページサイズのビット幅で and を取り、インデックスに変換(0~511)
	and	\tmp1, \tmp1, #PTRS_PER_TABLE - 1
	// ひとつのエントリのサイズは8バイトなので、インデックスを8倍(lsl #3)してオフセットに変換してから足す
	add	\tmp1, \tbl, \tmp1, lsl #3
	// ページの情報をアドレスの下位ビットにセット
	orr	\tbl, \next_tbl, #MM_TYPE_PAGE_TABLE
	// メモリに書き込む
	str	\tbl, [\tmp1]
.endm

//======== create_block_map ===============================================
// 実ページへのエントリを、上位のテーブルに追記するマクロ
// start と end で指定された範囲を tbl で指定されたテーブルに一気に追記していく
.macro	create_block_map, tbl, phys, start, end, flags, tmp1
	// 開始アドレスと終了アドレスをテーブル内のインデックスに変換
	// ページは 2MB 単位なので SECTION_SHIFT 分シフトしてマスクする
	lsr	\start, \start, #SECTION_SHIFT
	and	\start, \start, #PTRS_PER_TABLE - 1
	lsr	\end, \end, #SECTION_SHIFT
	and	\end, \end, #PTRS_PER_TABLE - 1
	// マップする物理アドレスをシフトしてから戻し、ページの先頭アドレスを作る
	// そのときに flags をセットして phys にテーブルエントリを作る(ページ属性は全て同じ)
	lsr	\phys, \phys, #SECTION_SHIFT
	mov	\tmp1, #\flags
	orr	\phys, \tmp1, \phys, lsl #SECTION_SHIFT
9999:
	// テーブルエントリを登録
	str	\phys, [\tbl, \start, lsl #3]
	// インデックスを1つ進める
	add	\start, \start, #1
	// 物理アドレスのほうは1ページ分(2MB)進める
	add	\phys, \phys, #SECTION_SIZE
	// end を超えたらループを抜ける、つまり start <= end の間はループする
	// よって end で指定されたアドレスはマップの対象となる
	cmp	\start, \end
	b.ls	9999b
.endm

//======== entry point ===============================================
.globl _start
_start:
	mrs	x0, mpidr_el1
	and	x0, x0,#0xFF		// Check processor id

	cmp x0, #0
	b.eq el3_entry		// Primary CPU

loop:
	// raspi の場合は PSCI がなく電源投入後に自動で全 CPU コアが起動する
	// CPU0 以外はここでループして initialized 変数がセットされるのを待つ

	// この OS はリンカスクリプトによって 0xffff_0000_0000_0000 に配置される
	// initialized も高位アドレスに配置されているため、そのまま ldr x1, [x0] はエラーになる
	ldr x0, =initialized
	and x0, x0, #0x00000000ffffffff
	ldr x1, [x0]
	cbz x1, loop
	dsb ish
	b el3_entry

el3_entry:
	bl get_el
	cmp	x0, #1
	b.eq el1_entry

	// EL3 から起動した場合(この OS がベアメタルで起動した場合)は最低限の設定だけして EL1 に切り替える
	ldr	x0, =SCTLR_VALUE_MMU_DISABLED
	msr	sctlr_el2, x0

	ldr	x0, =HCR_VALUE
	msr	hcr_el2, x0

	ldr	x0, =SCR_VALUE
	msr	scr_el3, x0

	// SPSR の M ビットを EL1h(0b101) に設定することで EL1 に直接遷移させる
	//   #define SPSR_MASK_ALL	(7 << 6)
	//   #define SPSR_EL1h		(5 << 0)
	//   #define SPSR_VALUE		(SPSR_MASK_ALL | SPSR_EL1h)
	ldr	x0, =SPSR_VALUE
	msr	spsr_el3, x0

	adr	x0, el1_entry
	msr	elr_el3, x0

	eret

el1_entry:
	// コア0でのみページテーブルの設定を実行する
	mrs	x0, mpidr_el1
	and	x0, x0, #0xFF
	cbz x0, page_table_setup
	b after_page_table_setup

page_table_setup:
	// MMU のキャッシュの無効化
	ldr	x0, =SCTLR_VALUE_MMU_DISABLED
	msr	sctlr_el1, x0

	// BSS セクションをゼロクリア
	adr	x0, bss_begin
	adr	x1, bss_end
	sub	x1, x1, x0
	bl 	memzero_without_stack

	// ページテーブルの作成
	bl 	__create_page_tables
	b after_page_table_setup

after_page_table_setup:
	mov x1, #LOW_MEMORY
	bl get_cpuid
	add x0, x0, #1
	mul x0, x1, x0
	mov x1, #VA_START
	add sp, x0, x1

	adrp	x0, pg_dir
	msr	ttbr1_el1, x0
	// 最も大事
	adrp	x0, pg_dir
	msr ttbr0_el1, x0

	ldr	x0, =(TCR_VALUE)
	msr	tcr_el1, x0

	ldr	x0, =(MAIR_VALUE)
	msr	mair_el1, x0

	// todo: objcopy はコード中のジャンプ先を修正しないため、kernel_main のアドレスがずれる
	ldr	x2, =kernel_main

	mov	x0, #SCTLR_MMU_ENABLED

    dsb ish
    isb
	msr	sctlr_el1, x0
    isb

	br 	x2

__create_page_tables:
	mov	x29, x30						// save return address

    // ページテーブル用の領域全体(PGD, PUD, PMD)をまずゼロクリア
	adrp	x0, pg_dir
#if defined(LOCAL_PERIPHERAL_BASE)
    // raspi3 の場合はローカルペリフェラルが別途必要なので4個分の領域をクリアする
	mov	x1, #(4 * PAGE_SIZE)
#else
	mov	x1, #(3 * PAGE_SIZE)
#endif
	bl 	memzero_without_stack

    // メインメモリに対する PGD と PUD を設定(PMD までのリンク構造を作成)
	adrp	x0, pg_dir
	mov	x1, #VA_START 
	create_pgd_pud_entry x0, x1, x2, x3

	// MMU が有効になった直後にクラッシュしないように追加で恒等マッピングも行う
	// PUD のアドレス(PGD の次のページ)を x2 にセット
	adrp x2, pg_dir
	add x2, x2, #PAGE_SIZE
	// 物理アドレスの開始位置のインデックスを計算して x1 にセット
	ldr x1, =RAM_BASE
	lsr x1, x1, #PUD_SHIFT
	and x1, x1, #PTRS_PER_TABLE - 1
	// メモリ属性を設定
	// x0 には PMD のアドレスが入っている
	orr x3, x0, #MM_TYPE_PAGE_TABLE
	// str 命令は x3 に入ったデータを、x2 が示すベースアドレスに x1 のオフセットを足したアドレスに書き込む
	// つまり、PUD に既存の PMD への参照を追加している
	// この PMD は先ほど作成したもので、メモリ空間を正しくマップしている
	str x3, [x2, x1, lsl #3]

	// PMD の中身(各エントリ)を作成
	// 1回目の create_block_map ではまずメインメモリをマップする
	// この時点でも x0 には PMD のアドレスが入っている
	ldr 	x1, =RAM_BASE							// マップ元の物理アドレスの開始位置
	ldr 	x2, =VA_START							// マップ先の仮想アドレスの開始位置
	ldr	x3, =(VA_START + RAM_SIZE - SECTION_SIZE)	// マップ先の仮想アドレスの終了位置(このアドレスを含んでマップされる)
	create_block_map x0, x1, x2, x3, MMU_FLAGS, x4

	// 2回目の create_block_map では周辺機器をマップする
	// この時点でも x0 には PMD のアドレスが入っている
	ldr 	x1, =DEVICE_BASE
	ldr 	x2, =(VA_START + DEVICE_BASE)
	ldr	x3, =(VA_START + DEVICE_BASE + DEVICE_SIZE - SECTION_SIZE)
	create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4
#else
	// raspi3 board
	// create_pgd_entry は PGD と PUD をひとつずつ作るため、1GB までしかマップされていない
	// 1GB 以降の領域 (Local Peripherals) をマップするために PUD にエントリを追加
	// x0 は現在 PMD1 (0-1GB用) のアドレスを指している
	// todo: マクロを使って PUD の作成処理を共通化したい
	adrp x2, pg_dir
	add x2, x2, #PAGE_SIZE
	ldr x1, =VA_START
	mov x4, #0x40000000
	add x1, x1, x4
	lsr x1, x1, #PUD_SHIFT
	and x1, x1, #PTRS_PER_TABLE - 1
	mov x4, #PAGE_SIZE
	add x3, x0, x4
	orr x3, x3, #MM_TYPE_PAGE_TABLE
	str x3, [x2, x1, lsl #3]

	/* Mapping kernel and init stack*/
	mov 	x1, xzr							// start mapping from physical offset 0
	mov 	x2, #VA_START						// first virtual address
	ldr	x3, =(VA_START + DEVICE_BASE - SECTION_SIZE)		// last virtual address
	create_block_map x0, x1, x2, x3, MMU_FLAGS, x4

	/* Mapping device memory*/
	mov 	x1, #DEVICE_BASE					// start mapping from device base address 
	ldr 	x2, =(VA_START + DEVICE_BASE)				// first virtual address
	ldr	x3, =(VA_START + PHYS_MEMORY_SIZE - SECTION_SIZE)	// last virtual address
	create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4
	add	x0, x0, #PAGE_SIZE

	/* Mapping peripheral memory */
    mov x1, #0x40000000
    ldr x2, =(VA_START + 0x40000000)
    ldr x3, =(VA_START + 0x40200000 - SECTION_SIZE)
    create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4
#endif

	mov	x30, x29						// restore return address
	ret
