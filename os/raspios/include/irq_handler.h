#ifndef _IRQ_HANDLER_H
#define _IRQ_HANDLER_H

void irq_vector_init(void);

void enable_peripheral_irqs(unsigned long cpuid);
void disable_peripheral_irqs(unsigned long cpuid);

#endif  /*_IRQ_HANDLER_H */
