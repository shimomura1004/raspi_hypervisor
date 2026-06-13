#include "board_config.h"
#include "utils.h"
#include "debug.h"
#include "timer.h"
#include "entry.h"
#include "mm.h"
#include "drivers/uart.h"

static const char *entry_error_messages[] = {
    "SYNC_INVALID_EL1t",
    "IRQ_INVALID_EL1t",
    "FIQ_INVALID_EL1t",
    "ERROR_INVALID_EL1T",

    "SYNC_INVALID_EL1h",
    "IRQ_INVALID_EL1h",
    "FIQ_INVALID_EL1h",
    "ERROR_INVALID_EL1h",

    "SYNC_INVALID_EL0_64",
    "IRQ_INVALID_EL0_64",
    "FIQ_INVALID_EL0_64",
    "ERROR_INVALID_EL0_64",

    "SYNC_INVALID_EL0_32",
    "IRQ_INVALID_EL0_32",
    "FIQ_INVALID_EL0_32",
    "ERROR_INVALID_EL0_32",

    "SYNC_ERROR",
    "SYSCALL_ERROR"
};

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address, unsigned long elr)
{
    WARN("%s, ESR_EL1: %x, FAR_EL1: %x, ELR_EL1(PC): %x", entry_error_messages[type], esr, address, elr);
}

void handle_irq(void)
{
    // generic timer の割り込みを確認
    unsigned long cntv_ctl;
    asm volatile("mrs %0, cntv_ctl_el0" : "=r"(cntv_ctl));
    // CNTP_CTL_EL0: Counter-timer Physical Timer Control register
    // [2] ISTATUS: The status of the timer.
    // [1] IMASK: Timer interrupt mask bit.
    // [0] ENABLE: Enables the timer.
    if (cntv_ctl & 0x4) {
        raspios_handle_timer_irq();
        return;
    }

    unsigned int irq = get32(PHYS_TO_VIRT(IRQ_PENDING_1));
    // UART の割り込みを確認
    if (irq & (1 << 29)) {
        handle_uart_irq();
        irq &= ~(1 << 29);
    }

    // // システムタイマの割り込みを確認
    // if (irq & SYSTEM_TIMER_IRQ_1_BIT) {
    //  raspios_handle_timer_irq();
    //  irq &= ~SYSTEM_TIMER_IRQ_1_BIT;
    // }

    if (irq) {
        WARN("Unknown pending irq: %x", irq);
    }
}
