#include "pm.h"
#include "utils.h"
#include "debug.h"

void system_shutdown() {
    // 実際に電源を落とすには mailbox でファームウェアに指示を送る必要がある
    // todo: PSCI で汎用化する
    INFO("System Halted.");

    // todo: これだけでは他の pCPU が停止しない
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
