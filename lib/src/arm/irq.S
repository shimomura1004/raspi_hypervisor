// ARM 共通の割込み制御処理

// 割込みを有効にする
.globl enable_irq
enable_irq:
    // daifclr は割込みマスクビットをクリアするために使うレジスタ
    // DAIF: debug, Asynchronous abort, IRQ, FIQ
    // 2ビット目(0b10)が割込みに対応、0 だと有効、1 だと無効
    msr     daifclr, #2
    // SPSel: stack pointer selector
    // 最下位ビットは SP の使い方を指示する
    // 0b0: SP_EL0 をどの EL でも使う
    // 0b1: SP_ELx をそれぞれの EL で使う
    //msr    SPSel, #1
    ret

// 割込みを無効にする
.globl disable_irq
disable_irq:
    msr    daifset, #2
    ret

// 割込みが有効かどうかを返す
//   https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/DAIF--Interrupt-Mask-Bits
// DAIF[7] IRQ mask bit
//   0b0: Exception not masked (Enabled)
//   0b1: Exception masked (Disabled)
.globl is_interrupt_enabled
is_interrupt_enabled:
    mrs x0, daif
    // 7ビット目が割込みマスクビット
    and x0, x0, #0x80
	// 割込みが有効ならマスクされていない、つまり 0
	// このとき非負の値を返したいので、
    eor x0, x0, #0x80
    ret
