#ifndef	_IRQ_H
#define	_IRQ_H

#include "arm/irq.h"

void irq_vector_init( void );

void enable_local_interrupt_controller(unsigned long cpuid);
void enable_legacy_interrupt_controller();

void enable_virtual_timer_irq(void);
void disable_virtual_timer_irq(void);

#endif  /*_IRQ_H */
