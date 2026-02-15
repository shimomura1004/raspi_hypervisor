#include "arm/mmu.h"
#include "arm/sysregs.h"

// sm/hv の間に二段階アドレス変換はないので、メモリ空間は手動で分離する必要がある
// 普通はデバイスツリーなどのパラメータ情報を使ってお互いが干渉しないようにする

// 物理アドレス空間のメモリ配置:
//   sm: 0x0-0x4000 (16KB)
//     .text, .data: 0x0000-0x2fff
//     shared log:   0x3000-0x33ff
//     el3 stack:    0x3400-0x3fff
//   hv: 0x80000- (raspi の標準的なロード位置)

// note: Secure/Normal world のメモリアクセスを制限するのは MMU ではなくメモリコントローラ側にある TZASC
// TZASC はメモリアドレスと NS ビットを見てアクセス可否を判断する
// TZASC を設定できるのは EL3 のみ
// note: Raspberry Pi 3 (BCM2837) には TZASC が搭載されていないのでアクセス制限ができない

#define HV_BASE 0x80000

.extern vectors

.section ".text.boot"

// SCR_EL3: secure configuration register: secure monitor
//          どの例外を EL3 でトラップするか指定する他、NS ビットの設定もできる
// SPSR_EL3: saved program status register: condition flag とか
//   condition flag: ゼロかどうか、オーバーフローしたか、…
// ELR_EL3: exception link register: eret で戻る先のアドレス

// リンカスクリプトで .text.boot セクションが 0 番地に置かれる
// ここが SM のスタートポイントになる
.globl _start
_start:
	// todo: もし単一コアでのみ実行する処理があれば他コアをここでとどめる
	b	el3_entry

el3_entry:
	// スタックポインタの設定 (SM 領域の末尾 0x3400- を使用)
	// 各コア用のスタックとして 3KB を確保
	// SP = 0x4000 - (core_id * 0x300)
	// todo: 定数を使わずリンカスクリプトなどでシンボルとして定義するべき
	mrs	x0, mpidr_el1
	and	x0, x0, #3
	ldr	x1, =0x4000
	mov	x2, #0x300
	mul	x2, x0, x2
	sub	x1, x1, x2
	mov	sp, x1

	// VBAR_EL3 にベクタテーブルのアドレスを設定
	adr	x0, vectors
	msr	vbar_el3, x0

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

	// Core 0 以外はログ出力をスキップして HV へ遷移
	mrs	x0, mpidr_el1
	and	x0, x0, #3
	cbnz	x0, jump_to_hv

	// 起動ログの出力
	bl sm_log_init
	bl print_boot_message

jump_to_hv:
	// EL2 に遷移しつつ HV_BASE にジャンプ
	ldr	x0, =HV_BASE
	msr	elr_el3, x0
	eret
