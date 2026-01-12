#ifndef _GENERIC_TIMER_H
#define _GENERIC_TIMER_H

// Core 0 Timers interrupt control
// [7] nCNTVIRQ FIQ control. If set, this bit overrides the IRQ bit (3).
// [6] nCNTHPIRQ FIQ control. If set, this bit overrides the IRQ bit (2). 
// [5] nCNTPNSIRQ FIQ control. If set, this bit overrides the IRQ bit (1).
// [4] nCNTPSIRQ FIQ control. If set, this bit overrides the IRQ bit (0). 
// [3] nCNTVIRQ IRQ control.
// [2] nCNTHPIRQ IRQ control.
// [1] nCNTPNSIRQ IRQ control.
// [0] nCNTPSIRQ IRQ control.
//   0b0: disabled
//   0b1: enabled
#define CORE0_TIMER_IRQCNTL 0x40000040
#define CORE1_TIMER_IRQCNTL 0x40000044 
#define CORE2_TIMER_IRQCNTL 0x40000048 
#define CORE3_TIMER_IRQCNTL 0x4000004C 

#define TIMER_IRQCNTL_CNTVIRQ_FIQ_ENABLED   (1 << 7)
#define TIMER_IRQCNTL_CNTHPIRQ_FIQ_ENABLED  (1 << 6)
#define TIMER_IRQCNTL_CNTPNSIRQ_FIQ_ENABLED (1 << 5)
#define TIMER_IRQCNTL_CNTPSIRQ_FIQ_ENABLED  (1 << 4)
#define TIMER_IRQCNTL_CNTVIRQ_IRQ_ENABLED   (1 << 3)
#define TIMER_IRQCNTL_CNTHPIRQ_IRQ_ENABLED  (1 << 2)
#define TIMER_IRQCNTL_CNTPNSIRQ_IRQ_ENABLED (1 << 1)
#define TIMER_IRQCNTL_CNTPSIRQ_IRQ_ENABLED  (1 << 0)

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

#endif
