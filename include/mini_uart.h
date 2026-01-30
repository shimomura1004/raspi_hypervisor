#ifndef	_MINI_UART_H
#define	_MINI_UART_H

struct vm_struct2;

void uart_init(void);
void handle_uart_irq(void);
void putc(void *p, char c);
int is_uart_forwarded_vm(struct vm_struct2 *vm);

#endif  /*_MINI_UART_H */
