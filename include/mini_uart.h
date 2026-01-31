#ifndef	_MINI_UART_H
#define	_MINI_UART_H

void uart_init(void);
void handle_uart_irq(void);
void putc(void *p, char c);

#endif  /*_MINI_UART_H */
