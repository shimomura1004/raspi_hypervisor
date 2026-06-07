#include "board_config.h"
#include "utils.h"

// GIC v2 では 32 以上の割込み番号はシステム共有のもの
//   0-15:    SGI(Software Generated interrupts)
//   16-31:   PPI(Private Peripheral Interrupts)
//   32-1019: SPI(Share Peripheral Interrupts)
// SGI/PPI は各 CPU で処理されるので配送先を考える必要はない
// SPI はどの CPU で処理してもいいので ITARGETSR で配送先を指定する

// GICD_ISENABLER は Set-Enable レジスタなので、有効化したいビットだけを 1 にして書き込めばよい
//   Set-Enable: 1 を書き込むと有効化され、0 を書き込むと変化しない(もとの値が保たれる)

// GIC CPU Interface の初期化 (各コアで実行)
void gicc_init(unsigned long gicc_base) {
    // 全ての優先度レベルの割込みを許可
    put32(gicc_base + GICC_PMR_OFFSET, 0xff);
    // CPU interface を有効化 (Enable Group 0)
    put32(gicc_base + GICC_CTLR_OFFSET, GICC_CTLR_ENABLE);
}

// Distributor の初期化 (システムで1回実行)
void gicd_init(unsigned long gicd_base) {
    put32(gicd_base + GICD_CTLR_OFFSET, GICD_CTLR_ENABLE);
}

// 指定した IRQ を特定の CPU にルーティングし、有効化する
void gic_enable_interrupt(unsigned long gicd_base, unsigned int irq_id, unsigned int cpu_mask) {
    if (irq_id >= 32) {
        // SPI(share peripheral interrupts) の場合、ターゲット CPU を設定 (GICv2)
        // ITARGETSR は 1レジスタに4つの割込み、各8ビットで CPU マスクを指定
        unsigned int reg_offset = GICD_ITARGETSR_OFFSET + (irq_id / 4) * 4;
        unsigned int shift = (irq_id % 4) * 8;
        unsigned int val = get32(gicd_base + reg_offset);
        val &= ~(0xff << shift);
        val |= (cpu_mask << shift);
        put32(gicd_base + reg_offset, val);
    }

    // ISENABLER は 1レジスタに32個の割込みを保持する
    // ID 0-31 は ISENABLER0, ID 32-63 は ISENABLER1... と続く
    unsigned int enable_offset = GICD_ISENABLER_OFFSET + (irq_id / 32) * 4;
    put32(gicd_base + enable_offset, (1 << (irq_id % 32)));
}

// 指定した IRQ を無効化する
void gic_disable_interrupt(unsigned long gicd_base, unsigned int irq_id) {
    // ICENABLER (Interrupt Clear-Enable Register) を使用
    unsigned int disable_offset = GICD_ICENABLER_OFFSET + (irq_id / 32) * 4;
    put32(gicd_base + disable_offset, (1 << (irq_id % 32)));
}

// static unsigned long base_gicc_address = GIC_CPU_BASE;
static unsigned long base_gicd_address = GIC_DIST_BASE;

void enable_virtual_timer_irq(void) {
    put32(base_gicd_address + GICD_ISENABLER_OFFSET, (1 << IRQ_VIRT_TIMER));
}

void disable_virtual_timer_irq(void) {
    put32(base_gicd_address + GICD_ICENABLER_OFFSET, (1 << IRQ_VIRT_TIMER));
}
