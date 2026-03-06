#ifndef	_MINI_UART_H
#define	_MINI_UART_H

#include "spinlock.h"

extern struct spinlock console_lock;

struct vm_struct2;

void uart_init(void);
void handle_uart_irq(void);
void putc(void *p, char c);
int is_uart_forwarded_vm(struct vm_struct2 *vm);
void switch_console_to_vm(int vmid);

#endif  /*_MINI_UART_H */
