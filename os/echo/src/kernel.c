#include "printf.h"
#include "utils.h"
#include "mini_uart.h"
#include "../../../hv/include/hypercall_type.h"

#define STR(x) #x
#define XSTR(x) STR(x)

void kernel_main(void)
{
	uart_init();
	init_printf(0, putc);
	int el = get_el();
	printf("Exception level: %d \r\n", el);
	printf("Press '!' to trigger exception\r\n");

	while (1) {
		char c = uart_recv();
		if (c == '!') {
			printf("Triggering exception...\n");
			asm volatile("hvc #" XSTR(HYPERCALL_TYPE_CAUSE_PANIC));
		}
		c = c == '\r' ? '\n' : c;
		uart_send(c);
	}
}
