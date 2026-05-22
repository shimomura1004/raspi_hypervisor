// implements lib/include/irq.h
#include "board_config.h"
#include "utils.h"

static const unsigned long timer_controls[] = {
    CORE0_TIMER_IRQCNTL_OFFSET,
    CORE1_TIMER_IRQCNTL_OFFSET,
    CORE2_TIMER_IRQCNTL_OFFSET,
    CORE3_TIMER_IRQCNTL_OFFSET
};

static const unsigned long mbox_controls[] = {
    MBOX_CORE0_CONTROL_OFFSET,
    MBOX_CORE1_CONTROL_OFFSET,
    MBOX_CORE2_CONTROL_OFFSET,
    MBOX_CORE3_CONTROL_OFFSET
};

// ベースアドレスは raspi3 向けに決め打ちなので、OS から仮想アドレスを渡してもらう必要はない
static unsigned long base_qa7_address = QA7_BASE;
static unsigned long base_irq_address = IRQ_BASE;

// 各コア個別の割込み設定 (Generic Timer, Mailbox など)
void enable_interrupt_controller(unsigned long cpuid)
{
    // Generic Timer (nCNTPNSIRQ) 割込みの有効化
    put32(base_qa7_address + timer_controls[cpuid], TIMER_IRQCNTL_CNTHPIRQ_IRQ_ENABLED | TIMER_IRQCNTL_CNTVIRQ_IRQ_ENABLED);

    // Mailbox 割込みの有効化
    // このレジスタは各コアが個別に持つ ARM Local interrupt controller (QA7) のもの
    // CPU コア間の通信に使う
    put32(base_qa7_address + mbox_controls[cpuid], 1);
}

// RPi は割込みの有効・無効の管理用に3つのレジスタを持つ
//   #define ENABLE_IRQS_1      (PBASE+0x0000B210)
//   #define ENABLE_IRQS_2      (PBASE+0x0000B214)
//   #define ENABLE_BASIC_IRQS  (PBASE+0x0000B218)
//   BASIC IRQS はローカル割込み用
// システム全体で共通の割込み設定 (UART, System Timer など)
// 通常は Core 0 が一度だけ実行する
void enable_legacy_interrupt_controller()
{
    // put32(P2V(ENABLE_IRQS_1), SYSTEM_TIMER_IRQ_1_BIT);
    // put32(P2V(ENABLE_IRQS_1), SYSTEM_TIMER_IRQ_3_BIT);
    put32(base_irq_address + ENABLE_IRQS_1_OFFSET, AUX_IRQ_BIT);

    // Mailbox 割込みを有効化 
    // こちらは BCM2837 の Legacy Interrupt Controller のもの
    // GPU との通信に使うもので、本来は有効化する必要はない
    put32(base_irq_address + ENABLE_BASIC_IRQS_OFFSET, MBOX_IRQ_BIT);
}

void enable_virtual_timer_irq(void) {
    unsigned long cpuid = get_cpuid();
    unsigned long cntl = get32(base_qa7_address + timer_controls[cpuid]);
    cntl |= TIMER_IRQCNTL_CNTVIRQ_IRQ_ENABLED;
    put32(base_qa7_address + timer_controls[cpuid], cntl);
}

void disable_virtual_timer_irq(void) {
    unsigned long cpuid = get_cpuid();
    unsigned long cntl = get32(base_qa7_address + timer_controls[cpuid]);
    cntl &= ~TIMER_IRQCNTL_CNTVIRQ_IRQ_ENABLED;
    put32(base_qa7_address + timer_controls[cpuid], cntl);
}
