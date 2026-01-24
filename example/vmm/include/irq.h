#ifndef _IRQ_H
#define _IRQ_H

void irq_vector_init(void);
void enable_interrupt_controller(void);
void enable_irq(void);
void disable_irq(void);

#endif
