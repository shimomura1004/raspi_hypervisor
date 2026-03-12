#include "board_config.h"
#include "console.h"
#include "utils.h"

/* PL011 UART Registers (relative to UART_BASE) */
#define UART_DR     (UART_BASE + 0x00)
#define UART_FR     (UART_BASE + 0x18)
#define UART_IBRD   (UART_BASE + 0x24)
#define UART_FBRD   (UART_BASE + 0x28)
#define UART_LCRH   (UART_BASE + 0x2c)
#define UART_CR     (UART_BASE + 0x30)
#define UART_IMSC   (UART_BASE + 0x38)
#define UART_ICR    (UART_BASE + 0x44)

/* Flag Register bits */
#define UART_FR_TXFF (1 << 5) /* Transmit FIFO full */
#define UART_FR_RXFE (1 << 4) /* Receive FIFO empty */

struct spinlock console_lock;

void console_init(void) {
    init_lock(&console_lock, "console");
    /* QEMU virt console is typically already initialized, 
     * but we can set basic parameters here if needed.
     * For now, we assume simple output is enough.
     */
    
    /* Disable UART */
    put32(UART_CR, 0);
    
    /* Clear interrupts */
    put32(UART_ICR, 0x7ff);
    
    /* Set baud rate (dummy values for QEMU) */
    put32(UART_IBRD, 1);
    put32(UART_FBRD, 0);
    
    /* 8 bits, FIFO enabled, 1 stop bit, no parity */
    put32(UART_LCRH, (3 << 5) | (1 << 4));
    
    /* Enable UART, TX, RX */
    put32(UART_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

static void _uart_send(char c) {
    /* Wait until transmit FIFO is not full */
    while (get32(UART_FR) & UART_FR_TXFF)
        ;
    put32(UART_DR, c);
}

void putc(void *p, char c) {
    if (c == '\n') {
        _uart_send('\r');
    }
    _uart_send(c);
}

void handle_console_irq(void) {}
int is_uart_forwarded_vm(struct vm_struct2 *vm) { return 0; }
void switch_console_to_vm(int vmid) {}
