#include "pm.h"
#include "debug.h"
#include "psci.h"

extern volatile int system_halted;

void system_shutdown() {
    INFO("System halting (virt)...");
    system_halted = 1;

    // PSCI SYSTEM_OFF を呼び出してシステム全体をシャットダウン
    psci_system_off_smc();

    halt_current_cpu();
}

void system_reboot() {
    INFO("System rebooting (virt)...");

    // PSCI SYSTEM_RESET を呼び出してシステム全体をリブート
    psci_system_reset_smc();

    while (1) { asm volatile("wfi"); }
}