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

void generic_timer_init(void);
void handle_generic_timer_irq(void);

#endif
