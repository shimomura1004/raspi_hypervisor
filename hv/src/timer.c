#include "generic_timer.h"
#include "timer.h"
#include "sched.h"
#include "utils.h"
#include "irq.h"

static unsigned int interval_ms = 50;

void hv_timer_init(unsigned int timer_interval_ms) {
    interval_ms = timer_interval_ms;

    enable_virtual_timer_interrupt_el2();
    set_next_tick_el2(interval_ms);
}

// ハイパーバイザ用の Generic Timer の割込みハンドラ
void handle_generic_timer_irq() {
    // unsigned long frequency;
    // asm volatile("mrs %0, cntfrq_el0" : "=r"(frequency));
    // unsigned long ticks = (frequency * interval_ms) / 1000;

    // // 次のタイマ割込みが発生するよう値を再セット
    // asm volatile("msr cnthp_tval_el2, %0" : : "r"(ticks));
    set_next_tick_el2(interval_ms);

    // この pCPU 上で動く vCPU を切り替える
    timer_tick();
}

void handle_virtual_timer_irq() {
    // 仮想タイマ割込みが発生したら、この割込みが発生し続けないようにマスクする
    // その後、このハンドラが終了し VM に復帰するときに set_cpu_virtual_interrupt で仮想割込みが設定される
    disable_virtual_timer_irq();

    // CNTV_CVAL_EL0 は CPU 固有のレジスタであり退避・復帰の対象になっている
    // よって、カウンタ値が CNTV_CVAL_EL0 を超えて仮想タイマ割込みが発生した場合は
    // それは今実行中のゲスト OS が設定したタイマに対する割込みであると判断できる
        
    // HCR_EL2 は全 VM で共通なので、VM の切り替え時に退避・復帰する必要はないが、
    // VI は復帰時の状態によっては変化するかもしれないので再評価する必要がある
    // たとえば復帰時には物理タイマが進んでいて仮想タイマ割込みが発生しているかもしれない、など
}

 // VM に入る直前に呼ぶことで、タイマの状態と割込みの状態を同期させる
int sync_virtual_timer_irq() {
    unsigned long cntv_ctl;
    asm volatile("mrs %0, cntv_ctl_el0" : "=r"(cntv_ctl));

    int pending = 0;

    if (cntv_ctl & CNTP_CTL_EL0_ISTATUS_MET) { // 仮想タイマが比較値を超えている場合
        // 割込み条件が満たされている場合
        // 物理割込みをマスクし、仮想割込みありと判定
        disable_virtual_timer_irq();
        pending = 1;
    } else {
        // 割込み条件が満たされていない場合
        // 物理割込みをアンマスクし、仮想割込みなしと判定
        enable_virtual_timer_irq();
        pending = 0;
    }

    return pending;
}
