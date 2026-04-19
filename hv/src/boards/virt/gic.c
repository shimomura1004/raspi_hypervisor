#include <inttypes.h>
#include "utils.h"
#include "board_config.h"
#include "generic_timer.h"
#include "irq.h"
#include "mm.h"
#include "console.h"

void enable_local_interrupt_controller(unsigned long cpuid) {
    // まず GIC の CPU interface を初期化

    // 全ての割込みを許可(0xff より小さい割込みをすべて許可)
    put32(P2V(GICC_PMR), 0xff);
    // CPU interface を有効化
    put32(P2V(GICC_CTLR), GICC_CTLR_ENABLE);

    // 次に GIC の Distributor を初期化
    // 有効化するのは別途 enable_legacy_interrupt_controller() で行う

    // GICD_ISENABLER は Set-Enable レジスタなので、有効化したいビットだけを 1 にして書き込めばよい
    //   Set-Enable: 1 を書き込むと有効化され、0 を書き込むと変化しない(もとの値が保たれる)
    put32(P2V(GICD_ISENABLER), (1 << IRQ_HYP_PHYS_TIMER) | (1 << IRQ_VIRT_TIMER));
}

void enable_legacy_interrupt_controller(void) {
    // GIC Distributor を有効化
    put32(P2V(GICD_CTLR), GICD_CTLR_ENABLE);

    // todo: GICD_ITARGETSR は GICv2 のためのもの、GICv3 以降では別の仕組みを使う
    // UART 割込みのターゲットを CPU 0 に設定
    // SPI 1 (ID 33) は (33 = 8x4+1) なので、GICD_ITARGETSR[8]  の 2 バイト目 (bits 8-15) に対応
    unsigned int target_reg = get32(P2V(GICD_ITARGETSR + 32));
    target_reg |= (0x01 << 8);
    put32(P2V(GICD_ITARGETSR + 32), target_reg);

    // UART 割込みを有効化
    // SPI 1 の割込み番号は IRQ_UART0(33) で、ISENABLER[1] に相当する
    put32(P2V(GICD_ISENABLER + 4), (1 << (IRQ_UART0 - 32)));
}

// handle_irq はベクタから呼び出される
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

void enable_virtual_timer_irq(void) {
    put32(P2V(GICD_ISENABLER), (1 << IRQ_VIRT_TIMER));
}

void disable_virtual_timer_irq(void) {
    put32(P2V(GICD_ICENABLER), (1 << IRQ_VIRT_TIMER));
}
