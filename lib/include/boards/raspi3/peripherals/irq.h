#ifndef	_P_IRQ_H
#define	_P_IRQ_H

#include "boards/raspi3/raspi3.h"

#define IRQ_BASIC_PENDING_OFFSET    (0x00)
#define IRQ_PENDING_1_OFFSET        (0x04)
#define IRQ_PENDING_2_OFFSET        (0x08)
#define FIQ_CONTROL_OFFSET	        (0x0C)
#define ENABLE_IRQS_1_OFFSET        (0x10)
#define ENABLE_IRQS_2_OFFSET        (0x14)
#define ENABLE_BASIC_IRQS_OFFSET    (0x18)
#define DISABLE_IRQS_1_OFFSET       (0x1C)
#define DISABLE_IRQS_2_OFFSET       (0x20)
#define DISABLE_BASIC_IRQS_OFFSET   (0x24)

#define IRQ_BASE                    (DEVICE_BASE + 0x0000B200)
#define IRQ_BASIC_PENDING           (IRQ_BASE + IRQ_BASIC_PENDING_OFFSET)
#define IRQ_PENDING_1               (IRQ_BASE + IRQ_PENDING_1_OFFSET)
#define IRQ_PENDING_2               (IRQ_BASE + IRQ_PENDING_2_OFFSET)
#define FIQ_CONTROL	                (IRQ_BASE + FIQ_CONTROL_OFFSET)
#define ENABLE_IRQS_1               (IRQ_BASE + ENABLE_IRQS_1_OFFSET)
#define ENABLE_IRQS_2               (IRQ_BASE + ENABLE_IRQS_2_OFFSET)
#define ENABLE_BASIC_IRQS           (IRQ_BASE + ENABLE_BASIC_IRQS_OFFSET)
#define DISABLE_IRQS_1              (IRQ_BASE + DISABLE_IRQS_1_OFFSET)
#define DISABLE_IRQS_2              (IRQ_BASE + DISABLE_IRQS_2_OFFSET)
#define DISABLE_BASIC_IRQS          (IRQ_BASE + DISABLE_BASIC_IRQS_OFFSET)

#define SYSTEM_TIMER_IRQ_0_BIT      (1 << 0)
#define SYSTEM_TIMER_IRQ_1_BIT      (1 << 1)
#define SYSTEM_TIMER_IRQ_2_BIT      (1 << 2)
#define SYSTEM_TIMER_IRQ_3_BIT      (1 << 3)
#define AUX_IRQ_BIT                 (1 << 29)

#define MBOX_IRQ_BIT                (1 << 1)
#define PENDING_REGISTER_1_BIT      (1 << 8)
#define PENDING_REGISTER_2_BIT      (1 << 9)

void clear_timer_irq(void);

#endif  /*_P_IRQ_H */
