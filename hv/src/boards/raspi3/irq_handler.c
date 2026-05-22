#include "board_config.h"
#include "common_utils.h"
#include "mm.h"
#include "debug.h"
#include "peripherals/mailbox.h"
#include "generic_timer.h"

static const unsigned int irq_sources[] = {
    CORE0_IRQ_SOURCE,
    CORE1_IRQ_SOURCE,
    CORE2_IRQ_SOURCE,
    CORE3_IRQ_SOURCE
};

static const unsigned int mbox_rd_clrs[] = {
    MBOX_CORE0_RD_CLR_0,
    MBOX_CORE1_RD_CLR_0,
    MBOX_CORE2_RD_CLR_0,
    MBOX_CORE3_RD_CLR_0
};

// LIC(Legacy Interrupt Controller) が発生させる割込みを処理する
static void handle_lic() {
    // todo: daifset で割込みを止めてもシステムタイマによる割込みが発生してしまう、なぜ？

    // todo: 現状、IRQ_BASIC_PENDIGN は8,9ビット目以外は実装されていない
    //       つまりシステムタイマと UART 以外の割込みは発生しない
    unsigned long basic_pending = get32(P2V(IRQ_BASIC_PENDING));

    if (basic_pending & PENDING_REGISTER_1_BIT) {
        unsigned int irq = get32(P2V(IRQ_PENDING_1));

        // // システムタイマ
        // if (irq & SYSTEM_TIMER_IRQ_1_BIT) {
        //     irq &= ~SYSTEM_TIMER_IRQ_1_BIT;
        //     handle_systimer1_irq();
        // }
        // if (irq & SYSTEM_TIMER_IRQ_3_BIT) {
        //     irq &= ~SYSTEM_TIMER_IRQ_3_BIT;
        //     handle_systimer3_irq();
        // }

        // UART
        if (irq & AUX_IRQ_BIT) {
            irq &= ~AUX_IRQ_BIT;
            handle_console_irq();
        }

        // それ以外の割込みはエラーとする
        if (irq) {
            WARN("unknown pending irq: %x", irq);
        }
    }
    if (basic_pending & PENDING_REGISTER_2_BIT) {
        unsigned int irq = get32(P2V(IRQ_PENDING_2));
        if (irq) {
            WARN("unknown pending irq: %x", irq);
        }
    }
}

// 割込みベクタからジャンプしてくる先
// RPi3B では外部割込みはどれかひとつのコアでしか受信できない(デフォルトはコア0)
//   https://github.com/s-matyukevich/raspberry-pi-os/blob/master/docs/lesson03/linux/interrupt_controllers.md
//   "by default local interrupt controller is configured in such a way that all external interrupts are sent to the first core"
void handle_irq(void)
{
    unsigned long cpuid = get_cpuid();
    unsigned long source = get32(P2V(irq_sources[cpuid]));

    // Mailbox
    {
        // mailbox が割込みを発生させると basic_irq のビットが立つはずだが、そうなっていない
        // よって mailbox のソースを直接確認する
        if (source & IRQ_SOURCE_MBOX_0_BIT) {
            put32(P2V(mbox_rd_clrs[cpuid]), 0x1);
            handle_mailbox_irq(cpuid);
        }
    }

    // Generic Timer (Local Timer)
    {
        if (source & TIMER_IRQCNTL_CNTHPIRQ_IRQ_ENABLED) {
            handle_generic_timer_irq();
        }
        if (source & TIMER_IRQCNTL_CNTVIRQ_IRQ_ENABLED) {
            handle_virtual_timer_irq();
        }
    }

    // コア0のみでしか処理できない割込みの対応
    //   Legacy Interrupt Controller (UART/System timer)の割込みは Core0 にのみルーティングされる
    //   IRQ_BASIC_PENDING は全コアから見えるため、たとえばタイマ割込みと UART 割込みが同時に発生すると
    //   コア0以外のコアが UART を処理してしまう可能性がある
    if (cpuid == 0) {
        handle_lic();
    }
}
