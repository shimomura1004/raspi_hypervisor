#include "boards/raspi3/delays.h"
#include "peripherals/systimer.h"
#include "mm.h"
#include "utils.h"

/**
 * Get System Timer's counter
 */
unsigned long get_system_timer() {
    unsigned int h = -1, l;
    // we must read MMIO area as two separate 32 bit reads
    h = get32(P2V(TIMER_CHI));
    l = get32(P2V(TIMER_CLO));
    // we have to repeat it if high word changed during read
    if (h != get32(P2V(TIMER_CHI))) {
        h = get32(P2V(TIMER_CHI));
        l = get32(P2V(TIMER_CLO));
    }
    // compose long int value
    return ((unsigned long)h << 32) | l;
}

/**
 * Wait N microsec (with BCM System Timer)
 */
void wait_msec_st(unsigned int n) {
    unsigned long t = get_system_timer();
    // we must check if it's non-zero, because qemu does not emulate
    // system timer, and returning constant zero would mean infinite loop
    if (t) {
        while (get_system_timer() < t + n)
            ;
    }
}
