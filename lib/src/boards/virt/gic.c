// implements lib/include/irq.h
#include "board_config.h"
#include "utils.h"

// これらのオフセットは virt 専用
// ベースアドレスは virt 向けに決め打ちなので、OS から仮想アドレスを渡してもらう必要はない
static unsigned long base_gicc_address = GIC_CPU_BASE;
static unsigned long base_gicd_address = GIC_DIST_BASE;

// 各 CPU で実行する
void enable_interrupt_controller(unsigned long cpuid) {
    // まず GIC の CPU interface を初期化

    // 全ての割込みを許可(0xff より小さい割込みをすべて許可)
    put32(base_gicc_address + GICC_PMR_OFFSET, 0xff);
    // CPU interface を有効化
    put32(base_gicc_address + GICC_CTLR_OFFSET, GICC_CTLR_ENABLE);

    // 次に GIC の Distributor を初期化
    // 有効化するのは別途 enable_legacy_interrupt_controller() で行う

    // GICD_ISENABLER は Set-Enable レジスタなので、有効化したいビットだけを 1 にして書き込めばよい
    //   Set-Enable: 1 を書き込むと有効化され、0 を書き込むと変化しない(もとの値が保たれる)
    put32(base_gicd_address + GICD_ISENABLER_OFFSET, (1 << IRQ_HYP_PHYS_TIMER) | (1 << IRQ_VIRT_TIMER));
}

// システムで1回実行する
void enable_legacy_interrupt_controller(void) {
    // GIC Distributor を有効化
    put32(base_gicd_address + GICD_CTLR_OFFSET, GICD_CTLR_ENABLE);

    // todo: GICD_ITARGETSR は GICv2 のためのもの、GICv3 以降では別の仕組みを使う
    // UART 割込みのターゲットを CPU 0 に設定
    // SPI 1 (ID 33) は (33 = 8x4+1) なので、GICD_ITARGETSR[8]  の 2 バイト目 (bits 8-15) に対応
    unsigned int target_reg = get32(base_gicd_address + GICD_ITARGETSR_OFFSET + 32);
    target_reg |= (0x01 << 8);
    put32(base_gicd_address + GICD_ITARGETSR_OFFSET + 32, target_reg);

    // UART 割込みを有効化
    // SPI 1 の割込み番号は IRQ_UART0(33) で、ISENABLER[1] に相当する
    put32(base_gicd_address + GICD_ISENABLER_OFFSET + 4, (1 << (IRQ_UART0 - 32)));
}

// todo: これはドライバの API として用意する
void enable_virtual_timer_irq(void) {
    put32(base_gicd_address + GICD_ISENABLER_OFFSET, (1 << IRQ_VIRT_TIMER));
}

void disable_virtual_timer_irq(void) {
    put32(base_gicd_address + GICD_ICENABLER_OFFSET, (1 << IRQ_VIRT_TIMER));
}
