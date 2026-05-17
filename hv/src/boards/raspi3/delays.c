#include "peripherals/systimer.h"

// systimer を使って n us 待つ
void wait_msec_st(unsigned int n) {
    unsigned long t = get_physical_systimer_count();
    // we must check if it's non-zero, because qemu does not emulate
    // system timer, and returning constant zero would mean infinite loop
    if (t) {
        while (get_physical_systimer_count() < t + n)
            ;
    }
}
