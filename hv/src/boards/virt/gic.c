#include "utils.h"
#include "board_config.h"
#include "generic_timer.h"
#include "irq.h"
#include "mm.h"
#include <inttypes.h>

// GIC では割込みは SGI, PPI, SPI, LPI に分かれている
//   SGI: Software Generated Interrupt (0-15)
//     ソフトが生成する割込みで、CPU 間の通信などに使われる   
//   PPI: Private Peripheral Interrupt (16-31)
//     各 CPU 固有の割込みで、タイマ割込みなどに使われる
//   SPI: Shared Peripheral Interrupt  (32-1019)
//     全ての CPU で共有される割込みで、UART などの外部デバイスの割込みに使われる
//   LPI: Local Peripheral Interrupt   (8192-) (GICv3 以降のみ)
//     メッセージベースの割込み
#define IRQ_ID_SGI_START   0
#define IRQ_ID_PPI_START   16
#define IRQ_ID_SPI_START   32

#define GICD_CTLR       (GIC_DIST_BASE + 0x000)
#define GICD_TYPER      (GIC_DIST_BASE + 0x004)
#define GICD_ISENABLER  (GIC_DIST_BASE + 0x100)
#define GICD_ICENABLER  (GIC_DIST_BASE + 0x180)
#define GICD_IPRIORITYR (GIC_DIST_BASE + 0x400)
#define GICD_ITARGETSR  (GIC_DIST_BASE + 0x800)
#define GICD_ICFG       (GIC_DIST_BASE + 0xc00)

#define GICD_CTLR_ENABLE (1 << 0)

// PMR: CPU interface priority mask register
//   https://developer.arm.com/documentation/ddi0601/2025-12/External-Registers/GICC-PMR--CPU-Interface-Priority-Mask-Register
//   この値より優先度の高い割込みのみ PE に通知される
//   [7:0] Priority
// IAR: Interrupt Acknowledge Register
//   https://developer.arm.com/documentation/ddi0601/2025-12/External-Registers/GICC-IAR--CPU-Interface-Interrupt-Acknowledge-Register
//   現在発生している割込みのうち優先度が最も高いものの割込み番号を返す
//   割込み番号を読み取ると GIC は自動的にその割込みを処理中とマークし、それより優先度の低い割込みは無視される
// EOIR: End of Interrupt Register
//   https://developer.arm.com/documentation/ihi0048/b/Programmers--Model/CPU-interface-register-descriptions/End-of-Interrupt-Register--GICC-EOIR
//   書き込んだ割込み番号の処理が終了したことを GIC に通知する
#define GICC_CTLR       (GIC_CPU_BASE + 0x000)
#define GICC_PMR        (GIC_CPU_BASE + 0x004)
#define GICC_IAR        (GIC_CPU_BASE + 0x00c)
#define GICC_EOIR       (GIC_CPU_BASE + 0x010)

// GICC_IAR の下位10桁が割込み番号を表す
#define GICC_IAR_ID_MASK    0x3ff
#define GIC_IRQ_ID_SPURIOUS 1023
#define GIC_IRQ_ID_NONE     1022

#define GICC_CTLR_ENABLE (1 << 0)

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
    put32(P2V(GICD_CTLR), GICD_CTLR_ENABLE);    /* Enable Distributor */
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
    } else if (irq >= IRQ_ID_SPI_START && irq < 1022) {
        // その他の IRQ (UART など) は今は無視
    }

    put32(P2V(GICC_EOIR), iar);
}

void enable_virtual_timer_irq(void) {
    put32(P2V(GICD_ISENABLER), (1 << IRQ_VIRT_TIMER));
}

void disable_virtual_timer_irq(void) {
    put32(P2V(GICD_ICENABLER), (1 << IRQ_VIRT_TIMER));
}
