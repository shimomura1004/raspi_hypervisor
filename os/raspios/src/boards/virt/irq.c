#include "utils.h"
#include "debug.h"
#include "generic_timer.h"
#include "entry.h"
#include "peripherals/irq.h"
#include "peripherals/pl011_uart.h"

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
	// Initialize GIC Distributor on core 0 only
	if (cpuid == 0) {
		put32(GICD_CTLR, 0);       // Disable distributor
		put32(GICD_ITARGETSR + 32, (1 << 0)); // Route SPI 1 (UART0) to CPU 0
		put32(GICD_ISENABLER + 4, (1 << 1)); // Enable SPI 1 (intid=33, 33/32=1, 33%32=1 -> offset 4, bit 1)
		put32(GICD_CTLR, 1);       // Enable distributor
	}

	// Enable GIC CPU interface
	put32(GICC_PMR, 0xff);         // Allow all priority levels
	put32(GICC_CTLR, 1);           // Enable CPU interface

	// Enable PPI for Virtual Timer (IRQ 27)
	// PPIs are enabled in GICD_ISENABLER0, which is banked per-CPU
	unsigned int val = get32(GICD_ISENABLER);
	put32(GICD_ISENABLER, val | (1 << 27));
}

void disable_interrupt_controller(unsigned long cpuid)
{
	// Disable Virtual Timer
	unsigned int val = get32(GICD_ICENABLER);
	put32(GICD_ICENABLER, val | (1 << 27));

	if (cpuid == 0) {
		// Disable UART interrupt
		put32(GICD_ICENABLER + 4, (1 << 1));
		put32(GICD_CTLR, 0); // Disable Distributor
	}

	put32(GICC_CTLR, 0); // Disable CPU interface
}

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address)
{
	WARN("%s, ESR_EL1: %x, address: %x", entry_error_messages[type], esr, address);
}

void handle_irq(void)
{
	unsigned int iar = get32(GICC_IAR);
	unsigned int irq = iar & 0x3ff;

	if (irq == 27) {
		handle_timer_irq();
	} else if (irq == 33) {
		handle_uart_irq();
	} else if (irq < 1022) {
		WARN("Unknown pending irq: %x", irq);
	}

	put32(GICC_EOIR, iar);
}

void clear_timer_irq(void)
{
	// virt with GIC handles it through proper EOI
}
