#ifndef _LIB_IRQ_H
#define _LIB_IRQ_H

#include "arm/irq.h"

void enable_interrupt_controller(unsigned long cpuid);
void disable_interrupt_controller(unsigned long cpuid);
void enable_legacy_interrupt_controller(void);
// todo: disable がない

void enable_virtual_timer_irq(void);
void disable_virtual_timer_irq(void);

#endif  /*_LIB_IRQ_H */
