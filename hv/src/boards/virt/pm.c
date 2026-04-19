#include "pm.h"
#include "debug.h"

extern volatile int system_halted;

void system_shutdown() {
    INFO("System halting (virt)...");
    system_halted = 1;

    // todo: PSCI を使用して QEMU にシャットダウンを通知する
    
    halt_current_cpu();
}

void system_reboot() {
    INFO("System rebooting (virt)...");
    
    // todo: PSCI を使用して QEMU にリブートを通知する
    
    while (1) { asm volatile("wfi"); }
}
