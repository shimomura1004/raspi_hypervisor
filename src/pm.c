#include "pm.h"
#include "utils.h"
#include "debug.h"
#include "peripherals/mailbox.h"

// todo: shutdown/reboot のいずれも、まず全 pCPU を安全に停止させる必要がある
//       停止のための関数 system_halt を定義し、shutdown/reboot は halt 後の処理を担当させる

// todo: static にしてアクセサを定義し、直接アクセスできないようにする
volatile int system_halted = 0;

// 実際に電源を落とすには mailbox でファームウェアに指示を送る必要がある
// todo: PSCI で汎用化する
void system_shutdown() {
    // todo: アクセサを使う
    system_halted = 1;

    // 他 pCPU で割込みが発生したときまでに確実に DRAM に system_halted の更新を反映させる
    asm volatile("dmb sy" ::: "memory");

    // 他のコアを停止させるために Mailbox 割込み(IPI)を送る
    unsigned long cpuid = get_cpuid();
    if (cpuid != 0) put32(MBOX_CORE0_SET_0, 1);
    if (cpuid != 1) put32(MBOX_CORE1_SET_0, 1);
    if (cpuid != 2) put32(MBOX_CORE2_SET_0, 1);
    if (cpuid != 3) put32(MBOX_CORE3_SET_0, 1);

    INFO("System halting...");
    INFO("CPU %d halted", cpuid);
    while (1) { asm volatile("wfi"); }
}

void system_reboot() {
    // PM のレジスタ書き込みによるシャットダウンでは、
    // 割込みを含む一部のコントローラやメモリがクリアされないので注意

    // Watchdog タイマを設定してリセットをトリガする
    put32(PM_RSTC, PM_PASSWORD | 0x20);
    put32(PM_WDOG, PM_PASSWORD | 10);

    while (1) { asm volatile("wfi"); }
}
