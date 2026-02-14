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
	// todo: SMC 命令などのハンドラはここに
	b .

irq_handler_aarch64:
	b .

fiq_handler_aarch64:
	b .

serror_handler_aarch64:
	b .
