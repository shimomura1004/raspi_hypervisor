#include "board_config.h"
#include "printf.h"
#include "utils.h"
#include "mm.h"
#include "drivers/uart.h"
#include "irq.h"
#include "hypercall_type.h"

void kernel_main(void)
{
#if defined(BOARD_RASPI3)
    uart_init(AUX_BASE, GPIO_BASE);
#elif defined(BOARD_VIRT)
    uart_init(UART_BASE, 0);
#endif
    init_printf(0, putc);

    irq_vector_init();
    enable_interrupt_controller(0);
    unmask_irq();

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
            printf("Triggering SMC #0 call...\r\n");
            issue_smc(0);
        }
        else {
            c = c == '\r' ? '\n' : c;
            uart_send(c);
        }
    }
}
