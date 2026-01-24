#include "utils.h"
#include "printf.h"
#include "mini_uart.h"
#include "peripherals/irq.h"

void enable_interrupt_controller(void)
{
	// UART (AUX int) を有効化する
	put32(ENABLE_IRQS_1, (1 << 29));
}

void handle_irq(void)
{
	unsigned int irq = get32(IRQ_PENDING_1);
	if (irq & (1 << 29)) {
		handle_uart_irq();
	}
}
