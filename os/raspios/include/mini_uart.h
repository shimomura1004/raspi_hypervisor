#ifndef	_MINI_UART_H
#define	_MINI_UART_H

#include "spinlock.h"

extern struct spinlock console_lock;

void uart_init ( void );
void putc ( void* p, char c );
void handle_uart_irq(void);

#endif  /*_MINI_UART_H */
