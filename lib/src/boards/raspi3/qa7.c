#include "board_config.h"
#include "utils.h"

static const unsigned long timer_controls[] = {
    CORE0_TIMER_IRQCNTL_OFFSET,
    CORE1_TIMER_IRQCNTL_OFFSET,
    CORE2_TIMER_IRQCNTL_OFFSET,
    CORE3_TIMER_IRQCNTL_OFFSET
};

static const unsigned long ipi_controls[] = {
    IPI_CORE0_CONTROL_OFFSET,
    IPI_CORE1_CONTROL_OFFSET,
    IPI_CORE2_CONTROL_OFFSET,
    IPI_CORE3_CONTROL_OFFSET
};

// RPi は割込みの有効・無効の管理用に3つのレジスタを持つ
//   #define ENABLE_IRQS_1      (PBASE+0x0000B210)
//   #define ENABLE_IRQS_2      (PBASE+0x0000B214)
//   #define ENABLE_BASIC_IRQS  (PBASE+0x0000B218)
//   BASIC IRQS はローカル割込み用

static unsigned long base_qa7_address = QA7_BASE;
void init_qa7(unsigned long qa7_virt_base) {
    base_qa7_address = qa7_virt_base;
}

// Generic Timer (nCNTPNSIRQ) 割込みの有効化
void arm_local_timer_enable(unsigned long cpuid) {
    put32(base_qa7_address + timer_controls[cpuid], TIMER_IRQCNTL_CNTHPIRQ_IRQ_ENABLED | TIMER_IRQCNTL_CNTVIRQ_IRQ_ENABLED);
}

// コア間割込み (IPI) 用 Mailbox 割込みの有効化
void arm_local_ipi_enable(unsigned long cpuid) {
    // このレジスタは各コアが個別に持つ ARM Local interrupt controller (QA7) のもの
    // CPU コア間の通信に使う
    put32(base_qa7_address + ipi_controls[cpuid], 1);
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
