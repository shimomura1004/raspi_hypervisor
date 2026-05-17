#include "board_config.h"
#include "utils.h"
#include "sched.h"
#include "debug.h"

// todo: bcm2837 専用なので名前を変える

static unsigned long irq_base_address;
static const unsigned int interval_ms = 200;

void timer_init(unsigned long irq_base_virt_address)
{
    irq_base_address = irq_base_virt_address;

    unsigned long frequency;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(frequency));
    unsigned long ticks = (frequency * interval_ms) / 1000;
    INFO("Timer frequency: %u MHz, ticks: %u", frequency / 1000 / 1000, ticks);
    asm volatile("msr cntv_tval_el0, %0" : : "r"(ticks));
    // enable virtual timer interrupt
    asm volatile("msr cntv_ctl_el0, %0" : : "r"(1));
}

// todo: これはハンドラであってタイマのコードではない
void handle_timer_irq()
{
    unsigned long frequency;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(frequency));
    unsigned long ticks = (frequency * interval_ms) / 1000;
    asm volatile("msr cntv_tval_el0, %0" : : "r"(ticks));

    // ハイパーバイザ環境では、cntv_tval_el0 への書き込みをトラップすることができないので、
    // ゲストが仮想タイマ割込みを処理したタイミングを自分で知ることができない
    // よって明示的に IRQ_PENDING_1 にアクセスすることで MMIO によるトラップを発生させ、
    // ハイパーバイザに仮想割り込みの状態を更新(クリア)させる
#if defined(BOARD_RASPI3)
    get32(irq_base_address + IRQ_PENDING_1_OFFSET);
#endif

    timer_tick();
}
