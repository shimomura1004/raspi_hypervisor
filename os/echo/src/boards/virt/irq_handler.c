#include "board_config.h"
#include "utils.h"
#include "drivers/uart.h"
#include "gic.h"

void handle_irq(void)
{
    unsigned int iar = get32(GIC_CPU_BASE + GICC_IAR_OFFSET);
    unsigned int irq_id = iar & GICC_IAR_ID_MASK;

    if (irq_id == IRQ_UART0) {
        // todo: これは virt ボード用のコードだから handle_console_irq() が正しいのでは
        handle_uart_irq();
    }

    put32(GIC_CPU_BASE + GICC_EOIR_OFFSET, iar);
}
