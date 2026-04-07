#ifndef _DRIVERS_UART_H
#define _DRIVERS_UART_H

void uart_init(void);
void uart_send(char c);
char uart_recv(void);
void handle_uart_irq(void);
void putc(void *p, char c);

#endif /* _DRIVERS_UART_H */
