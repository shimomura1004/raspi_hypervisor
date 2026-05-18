#include "board_config.h"
#include "mm.h"

void handle_irq(void) {
    unsigned int iar = get32(P2V(GICC_IAR));
    unsigned int irq = iar & GICC_IAR_ID_MASK;

    // 1022 (No pending interrupts) や 1023 (Spurious interrupt) のチェックを入れるとより安全
    //   IAR を読み込む瞬間に割込みがキャンセルされると 1023 が返ることがある
    //   secure/nonsecure の関係で、割込みが読み取れない場合に 1022 が返ることがある
    if (irq >= GIC_IRQ_ID_NONE) {
        return;
    }

    if (irq == IRQ_HYP_PHYS_TIMER) {
        handle_generic_timer_irq();
    } else if (irq == IRQ_VIRT_TIMER) {
        handle_virtual_timer_irq();
    } else if (irq == IRQ_UART0) {
        handle_console_irq();
    }

    put32(P2V(GICC_EOIR), iar);
}
