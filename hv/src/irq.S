.globl irq_vector_init
irq_vector_init:
	// SPSel: stack pointer select
	//   0b0: use SP_EL0 at all Exception levels
	//   0b1: use SP_ELx for Exception level ELx
	// SPSel の初期値は 1 なので不要
	// msr SPSel, #1

	adr	x0, vectors		// load VBAR_EL2 with virtual
	// vbar: vector base address register
	//   例外発生時に呼び出されるハンドラのアドレスを格納
	msr	vbar_el2, x0	// vector table address
	ret
