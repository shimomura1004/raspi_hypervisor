#include "board_config.h"
#include "utils.h"
#include "drivers/uart.h"

void handle_irq(void)
{
    unsigned int irq = get32(IRQ_PENDING_1);
    if (irq & (1 << 29)) {
        handle_uart_irq();
    }
}
