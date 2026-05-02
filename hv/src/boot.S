#include "arm/mmu.h"
#include "arm/sysregs_def.h"
#include "mm.h"
#include "peripherals/base.h"

.section ".text.boot"

// 初期化が終わったことを表すメモリ
.extern initialized_flag

// HCR_EL2: hypervisor configuration register: ハイパーバイザ関連の機能
// SCR_EL3: secure configuration register: secure monitor
//          どの例外を EL3 でトラップするか指定する他、NS ビットの設定もできる
// SPSR_EL3: saved program status register: condition flag とか
//   condition flag: ゼロかどうか、オーバーフローしたか、…
// ELR_EL3: exception link register: eret で戻る先のアドレス

// リンカスクリプトで .text.boot セクションが 0x80000 番地に置かれる
// ここが HV のスタートポイントになる
.globl _start
_start:
    // コア 0 のみが初期化を行う
	mrs	x0, mpidr_el1
	and	x0, x0,#0xFF		// Check processor id
	cbz	x0, check_el_state

	// コア 1-3 は initialized_flag がセットされるのを待って実行
	ldr x0, =initialized_flag

wait_loop:
	// コア 0 が hypervisor_main で sev 命令を実行するまで待つ
	wfe
	ldr x1, [x0]
	cbz x1, wait_loop

check_el_state:
	// 今の EL に応じたジャンプ先に飛ぶ(EL3 なら el3_entry、EL2 なら el2_entry)
	mrs	x0, CurrentEL
	lsr	x0, x0, #2
	cmp	x0, #3
	b.eq	el3_entry
	b	el2_entry

// EL3 の実質的なスタート
// 起動直後は EL3 になっているので、セキュアモニタがいない場合は必要な設定を行ったあと EL2 に切り替える
el3_entry:
	// RW(register width) は hcr_el2 と同じく 1 にしておかないと 32 ビットになる様子
	// NS(non-secure) は non-secure bit で、1 にすると
	//   Indicates that Exception levels lower than EL3 are
	//   in Non-secure state, so memory accesses
	//   from those Exception levels cannot access Secure memory.
	//   今回はそのまま Normal world に遷移したいので NS は 1 にする
	ldr	x0, =SCR_EL3_VALUE
	msr	scr_el3, x0

	// EL3 から eret で復帰する(起動後の EL3 から EL1 へ遷移する)ときの
	// 状態を調整している
	// カーネルの処理に遷移するので特権が必要かつ、EL1 で動く必要がある
	// よって EL1h をセットする
	ldr	x0, =SPSR_EL3_VALUE
	msr	spsr_el3, x0

	// boot.S 内にある el2_entry のアドレスを elr_el3 に入れる
	// elr_el3 は EL3 から eret するときに使われるアドレスを入れる
	adr	x0, el2_entry
	msr	elr_el3, x0

	// 指定した状態(EL2)で、指定したアドレス(el2_entry)に復帰
	eret

// ここから EL2 として実行される
el2_entry:
	// SCTLR_VALUE_MMU_DISABLED は sysregs.h にて定義されている
	// MMU を無効にした状態の規定値を x0 レジスタにロードする
	// 命令キャッシュ・データキャッシュも無効化される
	ldr	x0, =SCTLR_EL2_VALUE_MMU_DISABLED

	// EL2 用に設定を登録
	msr	sctlr_el2, x0

	// 二段階アドレス変換の有効化など
	ldr	x0, =HCR_EL2_VALUE
	msr	hcr_el2, x0

	isb

	// テーブルを用意するのはコア 0 のみ
	mrs	x0, mpidr_el1
	and	x0, x0, #0xFF
	cbz x0, page_table_setup
	b after_page_table_setup

page_table_setup:
	// リンカスクリプトで設定される bss 領域の先頭/末尾を x0/x1 に入れる
	adr	x0, bss_begin
	adr	x1, bss_end
	// bss 領域のサイズ(x1 - x0)を計算し x1 に入れる
	sub	x1, x1, x0
	// bss 領域を 0 クリア
	bl 	memzero_without_stack

	// EL3 では仮想アドレスは使用していない
	// 2段階アドレス変換は、TTBR0_EL1/TTBR1_EL1 -> VTTBR_EL2 の順で変換テーブルを使う
	// EL2 用のページテーブルを準備
	bl 	__create_page_tables

after_page_table_setup:
	// スタック領域の設定
	// SP = bss_end + (SECTION_SIZE * (CPUID + 1))
	// イメージの末尾(bss_end)の後ろにスタックを配置する
	// todo: x0 に bss_end を入れるタイミングがおかしいので修正する
	adr	x0, bss_end
	mov	x1, #SECTION_SIZE
	bl get_cpuid
	add	x2, x0, #1
	mul	x1, x1, x2
	add	x0, x0, x1
	// 最後に VA_START を足すことでカーネルの仮想アドレスにしている
	ldr x1, =VA_START
	// sp には仮想アドレスが入っているので MMU が有効になってからじゃないと使えない
	add sp, x0, x1

	// TTBR(translate table base register)は複数ある: TTBR{0,1}_el{0,1} + TTBR0_el{2,3}
	// TTBR は x86 の cr3 に相当する(risc-v のsatp)
	// TTBR0は 0~256TB までの48-bitの仮想アドレス空間を保持
	// TTBR1のほうは 0xFFFF0000_00000000 から 0xFFFF_FFFFFFFF までの 256TB を保持
	// TTBR0 と TTBR1 で仮想アドレスを分離し、それぞれにユーザアプリとカーネルを置く

	// pg_dir はリンカスクリプトで設定されている
	// __create_page_tables によって、pg_dir にはアドレス変換テーブルの最上位のページのアドレスが入っている
	// __create_page_tables が作るのは単純なリニアマッピング
	// adrp は address of page の略で、pg_dir のページアドレスを x0 に入れる
	adrp	x0, pg_dir
	msr	ttbr0_el2, x0

	// TCR レジスタは MMU を設定するためのレジスタ
	ldr	x0, =(TCR_VALUE)
	msr	tcr_el2, x0

	// VTCR(virtualization translation control register)
	// Stage2 アドレス変換の制御を行うレジスタ
	ldr	x0, =(VTCR_EL2_VALUE)
	msr	vtcr_el2, x0

	// MAIR の設定を行う(詳細は mmu.h)
	// MAIR: memory attribute indirection register
	ldr	x0, =(MAIR_VALUE)
	msr	mair_el2, x0

	// EL1 に関連するすべての TLB エントリをクリアする
	tlbi alle1		// TLB Invalidate, ALL EL1

	// MMU を有効にする前にジャンプ先の絶対(仮想)アドレスを控えておく
	// 単に br hypervisor_main とすると、PC に対する hypervisor_main のアドレスへの相対値でジャンプするが、
	// MMU が無効であった場合の相対位置にジャンプしてしまうのでずれる
	// 一度 ldr でアドレスをレジスタに入れると絶対値になり、これであれば MMU 有効化後に適切な値となる
	// todo: MMU が有効になっても相対値でジャンプできそうだけど…
	ldr	x2, =hypervisor_main

	// MMU を有効にする準備
	mov	x0, #SCTLR_EL2_VALUE_MMU_ENABLED
	// MMU を有効にする前にすべての命令を実行完了しておく
	dsb ish
	isb
	// MMU を有効にする
	msr	sctlr_el2, x0
	// これ以降、MMU が有効になった状態で命令が実行されることを確実にする
	isb

	// CPUID を第一引数に入れて hypervisor_main 関数にジャンプ
	bl get_cpuid
	br 	x2

//-------- ここからマクロ定義 ------------------------------------------------
// todo: できれば C にしたい

// AArch64 の仮想アドレス(48bit)の構成
//   [PGD: 9bit][PUD: 9bit][PMD: 9bit][PTE: 9bit][OFFSET: 12bit]
// RPi OS ではセクションマッピングを使う、セクションのサイズは 2MB
//   -> PTE がなくなり、オフセットが21bit(2MB)になる

// RPi3 の RAM は 1GB しかないので、PGD と PUD は1つずつで十分である
// ただ1GB以降の物理アドレスに存在するローカルペリフェラルも同様にリニアマップしたいので
// PMD をもう1つ追加する

// (raspi3 の場合)
// create_pgd_pud_entry を実行すると、4ページ分確保したメモリ領域に、前から順番に
//   [PGD] pg_dir が指すところ
//   [PUD]
//   [PMD] DRAM 用、未初期化
//   [PMD] ローカルペリフェラル用、未初期化
// が確保される
// またセクションマッピングを使うので PTE は不要

// #define PAGE_SHIFT	 		12
// #define TABLE_SHIFT 			9
// #define PGD_SHIFT			PAGE_SHIFT + 3*TABLE_SHIFT
// #define PUD_SHIFT			PAGE_SHIFT + 2*TABLE_SHIFT

//======== create_pgd_pud_entry ===============================================

// ページグローバルディレクトリとページアッパーディレクトリにエントリを登録する
// raspi3/virt の場合のどちらも、物理メモリ上に [PGD][PUD][PMD][PMD] の順に
// テーブルが並んでいることを前提にしている

.macro	create_pgd_pud_entry, tbl, virt, tmp1, tmp2, tmp3
	// PGD(page global directory) を作る
	// PGD は1エントリで512GBをカバーする
	// 第4引数が #1 になっているので、PUD はすぐ次のページに作られることになる
	create_table_entry \tbl, \virt, PGD_SHIFT, #1, \tmp1, \tmp2, \tmp3
	add	\tbl, \tbl, #PAGE_SIZE

	// PUD(page upper directory) を作る
	// PMD は1エントリで1GBをカバーする
	// todo: ボードごとに初期化処理をわけるか、デバイスツリーを読んで切り替える
#if defined(BOARD_RASPI3)
	// raspi3 用のPUD作成
	//   0x00000000: RAM / DEVICE (BCM Peripherals)
	//   0x40000000: LOCAL_PERIPHERAL (ARM Control)

	// 第4引数が #1 になっているので、PMD はすぐ次のページに作られることになる
	create_table_entry \tbl, \virt, PUD_SHIFT, #1, \tmp1, \tmp2, \tmp3

	// 2つめの PUD を作る(ローカルペリフェラル用)
	// 第4引数が #2 になっているので、先ほど作った PMD と連続して並ぶことになる
	mov \virt, #LOCAL_PERIPHERAL_BASE
	create_table_entry \tbl, \virt, PUD_SHIFT, #2, \tmp1, \tmp2, \tmp3

	// todo: このマクロ内で進めるのが適切か？
	add	\tbl, \tbl, #PAGE_SIZE
#elif defined(BOARD_VIRT)
	// virt 用のPUD作成
	//   0x08000000: DEVICE -> 1つめの PMD(0x00000000-0x3fffffff)
	//   0x40000000: RAM    -> 2つめの PMD(0x40000000-0x7fffffff)

	// DEVICE_BASE(0GB〜1GB空間)用
	mov \virt, #0
	create_table_entry \tbl, \virt, PUD_SHIFT, #1, \tmp1, \tmp2, \tmp3

	 // RAM_BASE(1GB〜2GB空間)用
	 // マッピングの設定は2GBまでしかないので、QEMU の引数で RAM を増やす場合はマッピングを増やす必要がある
	mov \virt, #RAM_BASE
	create_table_entry \tbl, \virt, PUD_SHIFT, #2, \tmp1, \tmp2, \tmp3

	// todo: このマクロ内で進めるのが適切か？
	add	\tbl, \tbl, #PAGE_SIZE
#endif
.endm

//======== create_table_entry ===============================================

// tbl:             エントリを更新するページテーブルのアドレス
// virt:            エントリに追加する仮想アドレス
// shift:           PGD/PUD/PMD のどの部分を更新するかを示すシフト量
// nextpage_offset: 追加するエントリが指す次ページのアドレスが、tbl から見て何ページ先にあるか
// tmp1,2,3:        作業用に使えるレジスタ

// str はストア命令
//   STR   Xt, [base, Xm {,LSL|SXTX {#0 | #3}} ]   // レジスタオフセット
//     Xt: メモリに保存する値を保持するレジスタ
//     base: 書き込み先のベースとなるアドレスを保持するレジスタ
//     Xm: ベースに対するオフセットを保持するレジスタ
//     lsl #3: オフセットを左に3ビットシフト(8倍)することを指示している

.macro	create_table_entry, tbl, virt, shift, nextpage_offset, tmp1, tmp2, tmp3
	// logical shift right
	lsr	\tmp1, \virt, #\shift
	// テーブルインデックスの部分だけマスクして tmp1 に取り出す
	and	\tmp1, \tmp1, #PTRS_PER_TABLE - 1			// table index
	// tmp2 に次のページのアドレスを入れる
    mov \tmp2, #PAGE_SIZE
    mov \tmp3, \nextpage_offset
    mul \tmp2, \tmp2, \tmp3
	add	\tmp2, \tbl, \tmp2
	// アドレスの下位2ビットにフラグを設定し、これがページテーブルであると宣言
	orr	\tmp2, \tmp2, #MM_TYPE_PAGE_TABLE

	// ページエントリを書き込む
	//   tmp1: 更新したいエントリのインデックス(アドレスにするために8倍(lsl #3)する必要がある)
    //   tmp2: 書き込む値(PGD を更新するときは PUD のアドレス、PUD を更新するときは PMD のアドレス)
	str	\tmp2, [\tbl, \tmp1, lsl #3]
.endm

//======== create_block_map ===============================================

// PMD テーブルのエントリを準備する

// tbl:     PMD テーブルへのポインタ
// phys:    マップ対象の物理アドレスの開始位置
// start:   マップ先の仮想アドレスの先頭
// end:     マップ先の仮想アドレスの末尾
// flags:   エントリの lower attributes にセットするフラグ

.macro	create_block_map, tbl, phys, start, end, flags, tmp1
	// start のアドレスから PUD のインデックスを取り出す
	lsr	\start, \start, #SECTION_SHIFT
	and	\start, \start, #PTRS_PER_TABLE - 1			// table index
	// end のアドレスから PUD のインデックスを取り出す
	lsr	\end, \end, #SECTION_SHIFT
	and	\end, \end, #PTRS_PER_TABLE - 1				// table end index
	// 対象の物理アドレスを右シフトし、orr 命令で再度左シフトする
	// これにより2MB(21ビット)の境界にそろっていないアドレスでも端数を捨てて変換できるようにしている
	lsr	\phys, \phys, #SECTION_SHIFT
	mov	\tmp1, #\flags
	orr	\phys, \tmp1, \phys, lsl #SECTION_SHIFT			// table entry
	// この段階で、phys は2MBでアラインされ、かつ適切なフラグも設定されている状態になる

9999:
	// エントリをメモリに書き込み
	// create_table_entry と同じで、インデックスをアドレスに変換しつつ格納する
	str	\phys, [\tbl, \start, lsl #3]				// store the entry
	// start を進める(start はアドレスではなくインデックスなので1を足せばいい)
	add	\start, \start, #1					// next entry
	// 物理アドレスも進める
	add	\phys, \phys, #SECTION_SIZE				// next block
	// phys ではなくインデックスで比較するのは、phys の下位ビットにはフラグが設定されているため
	cmp	\start, \end
	// b.ls は less than or same なので、start と end が同じときも 9999 にジャンプする
	// なので end は SECTION_SIZE 分小さい値にしておかないとオーバーランする
	b.ls	9999b
.endm

//=============================================================================

// BCM2837 では RAM が物理アドレスの 0 番地から 1GB 搭載される
// そのうち 0x0000_0000 から 0x3F00_0000 までは DRAM
// 残りの 0x3F00_0000 から 0x4000_0000 まではデバイス
//   BCM2837 ARM Peripherals manual, section 1.2.3 ARM physical addresses
//     Physical addresses range from 0x3F000000 to 0x3FFFFFFF for peripherals.
//   https://cs140e.sergio.bz/docs/BCM2837-ARM-Peripherals.pdf
// それらを仮想アドレス VA_START から始めて DEVICE_BASE までの範囲をマッピングする
__create_page_tables:
	// x30 はリンクレジスタで、bl 命令でジャンプしたときの戻り先を保存している
	// このあと bl 命令でジャンプすると x30 が上書きされてしまうので控えている
	// x29 はだれも使わないことがわかっているのでスタックではなく x29 に控えれば十分
	mov	x29, x30						// save return address

	// pg_dir(リンカスクリプトで指定)のベースアドレスを x0 に代入
	adrp	x0, pg_dir

	// x1 にページディレクトリのサイズ(4KB x 4)を代入
	// #define PAGE_SHIFT	 		12
	// #define PAGE_SIZE   			(1 << PAGE_SHIFT)
	// #define PG_DIR_SIZE			(4 * PAGE_SIZE)
	mov	x1, #PG_DIR_SIZE

	// pg_dir が指す位置から4ページ分を 0 クリアする
	// PGD, PUD, PMDx2 の4つ分、RPi3 の DRAM は1GBで、2MB の倍数なのでセクションマッピングが使える
	// よって PTE はいらない
	bl 	memzero_without_stack

	// memzero で x0 の値は更新されるので改めて設定
	adrp	x0, pg_dir

	mov	x1, #VA_START
	// create_pgd_pud_entry は以下のように定義されたマクロ
	// 	.macro	create_pgd_pud_entry, tbl, virt, tmp1, tmp2
	// x0 で指定したページテーブルに、x1 のアドレス以降を使ったマッピングを作る
	// x2, x3 は作業用に自由に使っていいレジスタ
	create_pgd_pud_entry x0, x1, x2, x3, x4


#if defined(BOARD_RASPI3)
	// ---- 物理アドレス 0x0000_0000-0x3fff_ffff を仮想アドレス 0x0000_0000-0x3fff_ffff にマッピング ----
	// DRAM 領域のマッピング(リニアマッピング)
	mov x1, #RAM_BASE   // RAM の物理アドレス
	mov x2, #VA_START   // 仮想アドレスの開始位置 (今のところ raspi3/virt のどちらも RAM_BASE = VA_START)
	// これは、RAM の末尾にデバイスの領域が重なっている raspi3 を前提にしたコードになっている
	// todo: DEVICE_BASE を足しているが、これは VA_START が 0 であることを前提にしておりわかりにくい
	ldr	x3, =(VA_START + DEVICE_BASE - SECTION_SIZE)

	// create_block_map, tbl, phys, start, end, flags, tmp1
	// tbl = x0 = pg_dir
	// phys = x1 = RAM_BASE
	// start = x2 = VA_START
	// end = x3 = VA_START + DEVICE_BASE - SECTION_SIZE
	// flags = MMU_FLAGS
	// 1GB のうち 0x3f000000 以降はデバイス用の領域なので2回目の create_block_map 呼び出しで対応
	// end(x3) が SECTION_SIZE 分小さくなっているのは、
	//   start=end となったアドレスまでマップしてからループを抜けるようになっているため
	//   つまり1ブロック分小さくしておかないとオーバーランする
	create_block_map x0, x1, x2, x3, MMU_FLAGS, x4

	// デバイス領域のマッピング(リニアマッピング)
	mov x1, #DEVICE_BASE
	ldr x2, =(VA_START + DEVICE_BASE)
	// todo: DEVICE_BASE を足しているが、これは VA_START が 0 であることを前提にしておりわかりにくい
	ldr	x3, =(VA_START + PHYS_MEMORY_SIZE - SECTION_SIZE)
	create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4
	// ---- 物理アドレス 0x0000_0000-0x3fff_ffff を仮想アドレス 0x0000_0000-0x3fff_ffff にマッピング ----

	// エントリの追加対象を次の PMD に進める
	add	x0, x0, #PAGE_SIZE

	// ---- 物理アドレス 0x4000_0000-0x7fff_ffff を仮想アドレス 0x4000_0000-0x7fff_ffff にマッピング ----
    // ローカルペリフェラル領域のマッピング(リニアマッピング)
	// ローカルペリフェラル領域は 2MB 分で十分
    mov x1, #LOCAL_PERIPHERAL_BASE
    ldr x2, =(VA_START + LOCAL_PERIPHERAL_BASE)
    ldr x3, =(VA_START + LOCAL_PERIPHERAL_BASE + 0x00200000 - SECTION_SIZE)
    create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4
	// ---- 物理アドレス 0x4000_0000-0x7fff_ffff を仮想アドレス 0x4000_0000-0x7fff_ffff にマッピング ----
#elif defined(BOARD_VIRT)
	// ---- 物理アドレス 0x0000_0000-0x3fff_ffff を仮想アドレス 0x0000_0000-0x3fff_ffff にマッピング ----
	// デバイス領域のマッピング
	// virt の場合、デバイス領域は RAM より前にあり、サイズとしては 128MB(0x08000000) で十分
	//   GIC/UART/VirtIO など
	mov x1, #DEVICE_BASE
	ldr x2, =(DEVICE_BASE)
	ldr	x3, =(DEVICE_BASE + 0x08000000 - SECTION_SIZE)
	create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4
	// ---- 物理アドレス 0x0000_0000-0x3fff_ffff を仮想アドレス 0x0000_0000-0x3fff_ffff にマッピング ----

	// エントリの追加対象を次の PMD に進める
	add	x0, x0, #PAGE_SIZE

	// ---- 物理アドレス 0x4000_0000-0x7fff_ffff を仮想アドレス 0x4000_0000-0x7fff_ffff にマッピング ----
	// RAM 領域のマッピング
	// RAM 全体をマッピングしているが、QEMU の引数で RAM を 1GB 以上にする場合は PMD を増やす必要がある
	mov x1, #RAM_BASE
	ldr x2, =(VA_START)
	ldr	x3, =(VA_START + RAM_SIZE - SECTION_SIZE)
	create_block_map x0, x1, x2, x3, MMU_FLAGS, x4
	// ---- 物理アドレス 0x4000_0000-0x7fff_ffff を仮想アドレス 0x4000_0000-0x7fff_ffff にマッピング ----
#endif

	// x30 に戻りアドレスを入れて ret する
	mov	x30, x29						// restore return address
	ret
