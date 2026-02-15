#include "entry.h"

.macro	save_regs
	sub	sp, sp, #S_FRAME_SIZE
	stp	x0, x1, [sp, #16 * 0]
	stp	x2, x3, [sp, #16 * 1]
	stp	x4, x5, [sp, #16 * 2]
	stp	x6, x7, [sp, #16 * 3]
	stp	x8, x9, [sp, #16 * 4]
	stp	x10, x11, [sp, #16 * 5]
	stp	x12, x13, [sp, #16 * 6]
	stp	x14, x15, [sp, #16 * 7]
	stp	x16, x17, [sp, #16 * 8]
	stp	x18, x19, [sp, #16 * 9]
	stp	x20, x21, [sp, #16 * 10]
	stp	x22, x23, [sp, #16 * 11]
	stp	x24, x25, [sp, #16 * 12]
	stp	x26, x27, [sp, #16 * 13]
	stp	x28, x29, [sp, #16 * 14]
	add	x21, sp, #S_FRAME_SIZE
	mrs	x22, elr_el3
	mrs	x23, spsr_el3
	stp	x30, x21, [sp, #16 * 15]
	stp	x22, x23, [sp, #16 * 16]
.endm

.macro	restore_regs
	ldp	x30, x21, [sp, #16 * 15]
	ldp	x22, x23, [sp, #16 * 16]
	msr	elr_el3, x22
	msr	spsr_el3, x23
	ldp	x0, x1, [sp, #16 * 0]
	ldp	x2, x3, [sp, #16 * 1]
	ldp	x4, x5, [sp, #16 * 2]
	ldp	x6, x7, [sp, #16 * 3]
	ldp	x8, x9, [sp, #16 * 4]
	ldp	x10, x11, [sp, #16 * 5]
	ldp	x12, x13, [sp, #16 * 6]
	ldp	x14, x15, [sp, #16 * 7]
	ldp	x16, x17, [sp, #16 * 8]
	ldp	x18, x19, [sp, #16 * 9]
	ldp	x20, x21, [sp, #16 * 10]
	ldp	x22, x23, [sp, #16 * 11]
	ldp	x24, x25, [sp, #16 * 12]
	ldp	x26, x27, [sp, #16 * 13]
	ldp	x28, x29, [sp, #16 * 14]
	add	sp, sp, #S_FRAME_SIZE
	eret
.endm

.align 11
.globl vectors
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

// EL3 Exception Handlers
sync_handler:
	b .

irq_handler:
	b .

fiq_handler:
	b .

serror_handler:
	b .

sync_handler_aarch64:
	// SMC 命令などのハンドラ
	save_regs
	bl print_message
	restore_regs

irq_handler_aarch64:
	b .

fiq_handler_aarch64:
	b .

serror_handler_aarch64:
	b .
