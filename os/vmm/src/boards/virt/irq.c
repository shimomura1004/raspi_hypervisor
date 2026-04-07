#include "utils.h"
#include "printf.h"
#include "peripherals/irq.h"
#include "drivers/uart.h"

void enable_interrupt_controller(void)
{
	// Initialize GIC Distributor
	put32(GICD_CTLR, 0);       // Disable distributor
	put32(GICD_ITARGETSR + 32, (1 << 0)); // Route SPI 1 (UART0) to CPU 0
	put32(GICD_ISENABLER + 4, (1 << 1));  // Enable SPI 1 (intid=33)
	put32(GICD_CTLR, 1);       // Enable distributor

	// Enable GIC CPU interface
	put32(GICC_PMR, 0xff);         // Allow all priority levels
	put32(GICC_CTLR, 1);           // Enable CPU interface
}

void handle_irq(void)
{
	unsigned int iar = get32(GICC_IAR);
	unsigned int irq = iar & 0x3ff;

	if (irq == 33) {
		handle_uart_irq();
	}

	put32(GICC_EOIR, iar);
}
