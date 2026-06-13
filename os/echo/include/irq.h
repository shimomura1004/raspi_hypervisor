#ifndef _IRQ_H
#define _IRQ_H

#include "arm/irq.h"

void irq_vector_init(void);

void enable_interrupt_controller(void);

#endif
