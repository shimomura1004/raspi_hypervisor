#ifndef	_P_PL011_UART_H
#define	_P_PL011_UART_H

#include "peripherals/base.h"

#define UART_BASE   (PBASE + 0x09000000)

#define UART_DR     (UART_BASE + 0x00)
#define UART_FR     (UART_BASE + 0x18)
#define UART_IBRD   (UART_BASE + 0x24)
#define UART_FBRD   (UART_BASE + 0x28)
#define UART_LCRH   (UART_BASE + 0x2c)
#define UART_CR     (UART_BASE + 0x30)
#define UART_IMSC   (UART_BASE + 0x38)
#define UART_ICR    (UART_BASE + 0x44)

#define UART_FR_TXFF (1 << 5) /* Transmit FIFO full */
#define UART_FR_RXFE (1 << 4) /* Receive FIFO empty */

void uart_init(void);
void handle_uart_irq(void);

#endif  /*_P_PL011_UART_H */
