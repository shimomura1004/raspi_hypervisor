#include "board_config.h"
#include "utils.h"
#include "drivers/uart.h"
#include "gic.h"

// todo: 名前が lib 側とかぶっている？
void enable_interrupt_controller(void)
{
    // Distributor の初期化
    gicd_init(GIC_DIST_BASE);
    // UART 割込みを CPU 0 (mask=0x01) にルーティングして有効化
    gic_enable_interrupt(GIC_DIST_BASE, IRQ_UART0, 0x01);
    // CPU Interface の初期化
    gicc_init(GIC_CPU_BASE);
}

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
