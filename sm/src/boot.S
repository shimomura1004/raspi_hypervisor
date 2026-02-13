#include "arm/mmu.h"
#include "arm/sysregs.h"
/* #include "mm.h" */
/* #include "peripherals/base.h" */

#define HV_BASE 0x80000

.section ".text.boot"

// note: sm/hv の間に二段階アドレス変換はないので、メモリ空間は手動で分離する必要がある
//       普通はデバイスツリーなどのパラメータ情報を使ってお互いが干渉しないようにする
// todo: hv をアイデンティティマッピングからリニアマッピングに変更する
// todo: sm と hv を干渉しないメモリ領域に置く

// example:
//   sm: 0x0-0x4000 (16KB)
//   hv: 0x80000- (raspi の標準的なロード位置)
// todo: qemu がブートローダの機能を持っているので、起動スクリプトを修正し
//       hv/sm のバイナリをそれぞれロードするように変更する
//       hv/sm のどちらも flat-binary を qemu が直接メモリにロードする方法でいい

// 初期化が終わったことを表すメモリ
//.extern initialized_flag

// SCTLR_EL1: system control register: キャッシュや MMU が有効か無効か、など
// HCR_EL2: hypervisor configuration register: ハイパーバイザ関連の機能
// SCR_EL3: secure configuration register: secure monitor
//          どの例外を EL3 でトラップするか指定する他、NS ビットの設定もできる
// SPSR_EL3: saved program status register: condition flag とか
//   condition flag: ゼロかどうか、オーバーフローしたか、…
// ELR_EL3: exception link register: eret で戻る先のアドレス

// リンカスクリプトで .text.boot セクションが 0 番地に置かれる
// ここが OS のスタートポイントになる
.globl _start
_start:
	// todo: 仮実装、とりあえずジャンプ
 	b HV_BASE

    // コア 0 のみ初期化を行うため el3_entry へジャンプ
//	mrs	x0, mpidr_el1
//	and	x0, x0,#0xFF		// Check processor id
//	cbz	x0, el3_entry		// Hang for all non-primary CPU
//
//	// コア 1-3 は initialized_flag がセットされるのを待って実行
//	ldr x0, =initialized_flag
//
//wait_loop:
//	// コア 0 が hypervisor_main で sev 命令を実行するまで待つ
//	wfe
//	ldr x1, [x0]
//	cbz x1, wait_loop
//	b el3_entry

// EL3 の実質的なスタート
// 起動直後は EL3 になっているので、必要な設定を行ったあと EL2 に切り替える
el3_entry:
	// VBAR_EL3 にベクタテーブルのアドレスを設定
	adr	x0, vectors
	msr	vbar_el3, x0

	// sysregs.h にて定義されている
	// #define SCTLR_VALUE_MMU_DISABLED
	//   (SCTLR_RESERVED | SCTLR_EE_LITTLE_ENDIAN |
	//    SCTLR_I_CACHE_DISABLED | SCTLR_D_CACHE_DISABLED |
	//    SCTLR_MMU_DISABLED)
	// MMU を無効にした状態の規定値を x0 レジスタにロードする
	// 命令キャッシュ・データキャッシュも無効化される
	ldr	x0, =SCTLR_VALUE_MMU_DISABLED

	// EL2 用に設定を登録
	msr	sctlr_el2, x0

	// 二段階アドレス変換の有効化など
	ldr	x0, =HCR_VALUE
	msr	hcr_el2, x0

	// #define SCR_VALUE (SCR_RESERVED | SCR_RW | SCR_NS)
	// RW(register width) は hcr_el2 と同じく 1 にしておかないと 32 ビットになる様子
	// NS(non-secure) は non-secure bit で、1 にすると
	//   Indicates that Exception levels lower than EL3 are
	//   in Non-secure state, so memory accesses
	//   from those Exception levels cannot access Secure memory.
	//   今回はそのまま Normal world に遷移したいので NS は 1 にする
	ldr	x0, =SCR_VALUE
	msr	scr_el3, x0

	// CPTR_EL3: Architectural Feature Trap Register (EL3)
	// TFP(bit 10) を 0 にして、EL2/1 からの SIMD/FP 命令のトラップを無効にする
	mov x0, xzr
	msr cptr_el3, x0

	// SPSR_EL3 を設定して、eret で EL2h (EL2 + SP_ELx) に遷移するようにする
	// M[4]=0 (AArch64)
	// M[3:0]=1001 (EL2h)
	// F, I, A ビットをマスク (1)
	mov x0, #( (7 << 6) | (9 << 0) ) // SPSR_MASK_ALL | SPSR_EL2h
	msr	spsr_el3, x0
jump_to_hv:
	ldr	x0, =HV_BASE
	msr	elr_el3, x0

	// 指定した状態(EL2)で、指定したアドレス(el2_entry)に復帰
	eret

// todo: 別の場所に
.align 11
vectors:
	// Current EL with SP0
	b	sync_handler		// Synchronous
	b	irq_handler		// IRQ
	b	fiq_handler		// FIQ
	b	serror_handler		// SError

	// Current EL with SPx
	b	sync_handler
	b	irq_handler
	b	fiq_handler
	b	serror_handler

	// Lower EL using AArch64
	b	sync_handler_aarch64
	b	irq_handler_aarch64
	b	fiq_handler_aarch64
	b	serror_handler_aarch64

	// Lower EL using AArch32
	// 今回は AArch64 のみ対応


// EL3 Exception Handlers
// TODO: Implement handlers
sync_handler:
	b .

irq_handler:
	b .

fiq_handler:
	b .

serror_handler:
	b .

sync_handler_aarch64:
	// SMC命令など、下位のELからの同期例外を処理します
	b .

irq_handler_aarch64:
	b .

fiq_handler_aarch64:
	b .

serror_handler_aarch64:
	b .
