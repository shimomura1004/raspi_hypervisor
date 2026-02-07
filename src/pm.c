#include "pm.h"
#include "utils.h"

void system_shutdown() {
    // PM のレジスタ書き込みによるシャットダウンでは、割込みを含む一部のコントローラやメモリがクリアされない

    // Watchdog タイマを設定してリセットをトリガする
    put32(PM_RSTC, PM_PASSWORD | 0x20);
    put32(PM_WDOG, PM_PASSWORD | 10);

    while (1) { asm volatile("wfi"); }
}
