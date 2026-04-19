#include "pm.h"
#include "mm.h"
#include "debug.h"
#include "utils.h"
#include "boards/raspi3/pm.h"
#include "peripherals/mailbox.h"
#include "cpu_core.h"

extern volatile int system_halted;

// 実際に電源を落とすには mailbox でファームウェアに指示を送る必要がある
// todo: PSCI で汎用化する
void system_shutdown() {
    INFO("System halting...");
    // todo: shutdown/reboot のいずれも、まず全 pCPU を安全に停止させる必要がある
    //       ここで必要な終了処理を実施するべき

    // システムが停止状態になったことを示す
    system_halted = 1;

    // 他 pCPU で割込みが発生したときまでに確実に DRAM に system_halted の更新を反映させる
    asm volatile("dmb sy" ::: "memory");

    // 他のコアを停止させるために Mailbox 割込み(IPI)を送る
    unsigned long cpuid = get_cpuid();
    if (cpuid != 0) put32(P2V(MBOX_CORE0_SET_0), 1);
    if (cpuid != 1) put32(P2V(MBOX_CORE1_SET_0), 1);
    if (cpuid != 2) put32(P2V(MBOX_CORE2_SET_0), 1);
    if (cpuid != 3) put32(P2V(MBOX_CORE3_SET_0), 1);

    halt_current_cpu();
}

// PM のレジスタ書き込みによるシャットダウンでは、
// 割込みを含む一部のコントローラやメモリがクリアされないので注意
void system_reboot() {
    // Watchdog タイマを設定してリセットをトリガする
    put32(P2V(PM_RSTC), PM_PASSWORD | 0x20);
    put32(P2V(PM_WDOG), PM_PASSWORD | 10);

    while (1) { asm volatile("wfi"); }
}
