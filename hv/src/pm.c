#include "pm.h"
#include "debug.h"
#include "irq.h"
#include "cpu_core.h"

volatile int system_halted = 0;

int is_system_halted(void) {
    return system_halted;
}

void halt_current_cpu() {
    unsigned long cpuid = get_cpuid();
    INFO("CPU %d halted", cpuid);

    disable_irq();
    asm volatile("msr cnthp_ctl_el2, %0" : : "r"(0));
    while (1) { asm volatile("wfi"); }
}
