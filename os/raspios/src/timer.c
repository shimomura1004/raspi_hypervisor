#include "board_config.h"
#include "utils.h"
#include "sched.h"
#include "generic_timer.h"
#include "debug.h"

static unsigned long irq_base_address;
static unsigned int interval_ms = 200;

void raspios_timer_init(unsigned long irq_base_virt_address, unsigned int timer_interval_ms)
{
    irq_base_address = irq_base_virt_address;
    interval_ms = timer_interval_ms;

    enable_virtual_timer_interrupt();
    set_next_tick(interval_ms);
}

void raspios_handle_timer_irq()
{
    set_next_tick(interval_ms);

    // ハイパーバイザ環境では、cntv_tval_el0 への書き込みをトラップすることができないので、
    // ゲストが仮想タイマ割込みを処理したタイミングを自分で知ることができない
    // よって明示的に IRQ_PENDING_1 にアクセスすることで MMIO によるトラップを発生させ、
    // ハイパーバイザに仮想割り込みの状態を更新(クリア)させる
#if defined(BOARD_RASPI3)
    get32(irq_base_address + IRQ_PENDING_1_OFFSET);
#endif

    timer_tick();
}
