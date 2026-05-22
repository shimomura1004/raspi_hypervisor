#ifndef _QA7_REGS_H
#define _QA7_REGS_H

#define QA7_BASE                            (0x40000000)

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
#define CORE0_TIMER_IRQCNTL_OFFSET          (0x40)
#define CORE1_TIMER_IRQCNTL_OFFSET          (0x44) 
#define CORE2_TIMER_IRQCNTL_OFFSET          (0x48) 
#define CORE3_TIMER_IRQCNTL_OFFSET          (0x4C) 

#define CORE0_TIMER_IRQCNTL                 (QA7_BASE + CORE0_TIMER_IRQCNTL_OFFSET)
#define CORE1_TIMER_IRQCNTL                 (QA7_BASE + CORE1_TIMER_IRQCNTL_OFFSET)
#define CORE2_TIMER_IRQCNTL                 (QA7_BASE + CORE2_TIMER_IRQCNTL_OFFSET)
#define CORE3_TIMER_IRQCNTL                 (QA7_BASE + CORE3_TIMER_IRQCNTL_OFFSET)

#define TIMER_IRQCNTL_CNTVIRQ_FIQ_ENABLED   (1 << 7)
#define TIMER_IRQCNTL_CNTHPIRQ_FIQ_ENABLED  (1 << 6)
#define TIMER_IRQCNTL_CNTPNSIRQ_FIQ_ENABLED (1 << 5)
#define TIMER_IRQCNTL_CNTPSIRQ_FIQ_ENABLED  (1 << 4)
#define TIMER_IRQCNTL_CNTVIRQ_IRQ_ENABLED   (1 << 3)
#define TIMER_IRQCNTL_CNTHPIRQ_IRQ_ENABLED  (1 << 2)
#define TIMER_IRQCNTL_CNTPNSIRQ_IRQ_ENABLED (1 << 1)
#define TIMER_IRQCNTL_CNTPSIRQ_IRQ_ENABLED  (1 << 0)

#endif  /*_QA7_REGS_H */
