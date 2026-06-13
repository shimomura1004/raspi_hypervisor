#include "board_config.h"
#include "utils.h"

// LIC (Legacy Interrupt Controller) で Mini UART の割込みを有効化する
void lic_enable_aux(unsigned long irq_base) {
    put32(irq_base + ENABLE_IRQS_1_OFFSET, AUX_IRQ_BIT);
}

// GPU と CPU の間の Mailbox 割込みを有効化する
void lic_enable_mbox(unsigned long irq_base) {
    put32(irq_base + ENABLE_BASIC_IRQS_OFFSET, MBOX_IRQ_BIT);
}

// BCM2837 のシステムタイマの割込みを有効化する
void lic_enable_systimer(unsigned long irq_base) {
    put32(irq_base + ENABLE_IRQS_1_OFFSET, SYSTEM_TIMER_IRQ_1_BIT);
    put32(irq_base + ENABLE_IRQS_1_OFFSET, SYSTEM_TIMER_IRQ_3_BIT);
}
