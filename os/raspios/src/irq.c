#include "utils.h"
#include "debug.h"
#include "generic_timer.h"
#include "entry.h"
#include "peripherals/irq.h"
#include "mini_uart.h"

const char *entry_error_messages[] = {
	"SYNC_INVALID_EL1t",
	"IRQ_INVALID_EL1t",		
	"FIQ_INVALID_EL1t",		
	"ERROR_INVALID_EL1T",		

	"SYNC_INVALID_EL1h",		
	"IRQ_INVALID_EL1h",		
	"FIQ_INVALID_EL1h",		
	"ERROR_INVALID_EL1h",		

	"SYNC_INVALID_EL0_64",		
	"IRQ_INVALID_EL0_64",		
	"FIQ_INVALID_EL0_64",		
	"ERROR_INVALID_EL0_64",	

	"SYNC_INVALID_EL0_32",		
	"IRQ_INVALID_EL0_32",		
	"FIQ_INVALID_EL0_32",		
	"ERROR_INVALID_EL0_32",

	"SYNC_ERROR",
	"SYSCALL_ERROR"
};

void enable_interrupt_controller(unsigned long cpuid)
{
	// generic timer の割込みを有効化する
	// todo: BCM2837 には GIC がないため専用の割込みコントローラの設定が必要で、ゲスト環境ではトラップして処理する必要あり
	put32(CORE0_TIMER_IRQCNTL + 4 * cpuid, TIMER_IRQCNTL_CNTHPIRQ_IRQ_ENABLED | TIMER_IRQCNTL_CNTVIRQ_IRQ_ENABLED);

	// SYSTEM TIMER IRQ 1 を有効化する
	// put32(ENABLE_IRQS_1, SYSTEM_TIMER_IRQ_1);

	// UART を有効化する
	put32(ENABLE_IRQS_1, (1 << 29));
}

void disable_interrupt_controller(unsigned long cpuid)
{
	// generic timer の割込みを無効化する
	put32(CORE0_TIMER_IRQCNTL + 4 * cpuid, 0);

	// システムタイマーの割り込みを無効化する
	// put32(DISABLE_IRQS_1, SYSTEM_TIMER_IRQ_1);

	// UART の割り込みを無効にする
	put32(DISABLE_IRQS_1, (1 << 29));
}

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address)
{
	WARN("%s, ESR_EL1: %x, address: %x", entry_error_messages[type], esr, address);
}

void handle_irq(void)
{
	// generic timer の割り込みを確認
	unsigned long cntv_ctl;
	asm volatile("mrs %0, cntv_ctl_el0" : "=r"(cntv_ctl));
	// CNTP_CTL_EL0: Counter-timer Physical Timer Control register
	// [2] ISTATUS: The status of the timer.
	// [1] IMASK: Timer interrupt mask bit.
	// [0] ENABLE: Enables the timer.
	if (cntv_ctl & 0x4) {
		handle_timer_irq();
		return;
	}

	unsigned int irq = get32(IRQ_PENDING_1);
	// UART の割り込みを確認
	if (irq & (1 << 29)) {
		handle_uart_irq();
		irq &= ~(1 << 29);
	}

	// // システムタイマの割り込みを確認
	// if (irq & SYSTEM_TIMER_IRQ_1) {
	// 	handle_timer_irq();
	// 	irq &= ~SYSTEM_TIMER_IRQ_1;
	// }

	if (irq) {
		WARN("Unknown pending irq: %x", irq);
	}
}
