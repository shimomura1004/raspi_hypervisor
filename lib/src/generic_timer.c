#include "utils.h"

void enable_virtual_timer_interrupt() {
    asm volatile("msr cntv_ctl_el0, %0" : : "r"(1));
}

void set_next_tick(unsigned int interval_ms)
{
    unsigned long frequency;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(frequency));
    unsigned long ticks = (frequency * interval_ms) / 1000;
    asm volatile("msr cntv_tval_el0, %0" : : "r"(ticks));
}
