#ifndef	_IRQ_H
#define	_IRQ_H

#include "arm/irq.h"

void irq_vector_init( void );

void enable_interrupt_controller(unsigned long cpuid);
void disable_interrupt_controller(unsigned long cpuid);

#endif  /*_IRQ_H */
