#ifndef _GENERIC_TIMER_H
#define _GENERIC_TIMER_H

// CNTP_CTL_EL0: Counter-timer Physical Timer Control register
// Control register for the EL1 physical timer.
// https://developer.arm.com/documentation/ddi0601/2025-12/AArch64-Registers/CNTP-CTL-EL0--Counter-timer-Physical-Timer-Control-Register
// [2] ISTATUS: The status of the timer.
//   0b0: Timer condition is not met.
//   0b1: Timer condition is met.
// [1] IMASK: Timer interrupt mask bit.
//   0b0: Timer interrupt is not masked by the IMASK bit.
//   0b1: Timer interrupt is masked by the IMASK bit.
// [0] ENABLE: Enables the timer.
//   0b0 Timer disabled.
//   0b1 Timer enabled.
#define CNTP_CTL_EL0_ISTATUS_MET        (1 << 2)
#define CNTP_CTL_EL0_IMASK_MASKED       (1 << 1)
#define CNTP_CTL_EL0_ENABLE_ENABLED     (1 << 0)

void generic_timer_init(void);
void handle_generic_timer_irq(void);
void handle_virtual_timer_irq(void);
int sync_virtual_timer_irq(void);
void disable_virtual_timer_irq(void);

#endif
