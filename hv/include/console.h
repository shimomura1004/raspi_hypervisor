#ifndef _CONSOLE_H
#define _CONSOLE_H

#include "spinlock.h"

extern struct spinlock console_lock;

struct vm_struct2;

void console_init(void);
void putc(void *p, char c);
void handle_console_irq(void);
int is_uart_forwarded_vm(struct vm_struct2 *vm);
void switch_console_to_vm(int vmid);

#endif /* _CONSOLE_H */
