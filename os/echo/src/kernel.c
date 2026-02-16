#include "printf.h"
#include "utils.h"
#include "mini_uart.h"
#include "../../../hv/include/hypercall_type.h"

void kernel_main(void)
{
	uart_init();
	init_printf(0, putc);
	int el = get_el();
	printf("Exception level: %d \r\n", el);
	printf("Press '!' to trigger exception\r\n");
	printf("Press '#' to trigger SMC call\r\n");

	while (1) {
		char c = uart_recv();
		if (c == '!') {
			printf("Triggering exception...\r\n");
			issue_hvc(HYPERCALL_TYPE_CAUSE_PANIC);
		}
		else if (c == '#') {
			printf("Triggering SMC call...\r\n");
			// issue_smc();
		}
		else {
			c = c == '\r' ? '\n' : c;
			uart_send(c);
		}
	}
}
