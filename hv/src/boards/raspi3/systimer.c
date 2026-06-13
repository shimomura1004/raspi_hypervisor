#include "mm.h"
#include "printf.h"
#include "peripherals/systimer_regs.h"
#include "debug.h"

// // VM スイッチ用
// void handle_systimer1_irq() {
//  // 定期的に呼び出されるよう、次の比較値をセットする
//  put32(P2V(SYSTIMER_C1), get32(P2V(SYSTIMER_CLO)) + interval);
//  // 割込みをクリア
//  put32(P2V(SYSTIMER_CS), SYSTIMER_CS_M1);

//  // CPU0 以外のコアに mbox 割込みを送ってタスクを切り替えさせる
//  put32(P2V(IPI_CORE1_SET_0), 0x1);
//  put32(P2V(IPI_CORE2_SET_0), 0x1);
//  put32(P2V(IPI_CORE3_SET_0), 0x1);

//  // CPU0 の VM 切り替え
//  // このあと yield してしばらく帰ってこなくなるので最後に timer_tick() を呼ぶ
//  timer_tick();
// }

// // VM の割込み用
// void handle_systimer3_irq() {
//  // 割込みをクリア
//  put32(P2V(SYSTIMER_CS), SYSTIMER_CS_M3);
// }

// todo: これは hv に残す
void show_systimer_info() {
    acquire_lock(&console_lock);
    printf("HI: 0x%x\nLO: 0x%x\nCS: 0x%x\nC1: 0x%x\nC3: 0x%x\n",
    get32(P2V(SYSTIMER_CHI)), get32(P2V(SYSTIMER_CLO)), get32(P2V(SYSTIMER_CS)), get32(P2V(SYSTIMER_C1)), get32(P2V(SYSTIMER_C3)));
    release_lock(&console_lock);
}
