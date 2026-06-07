#ifndef _LIB_GIC_H
#define _LIB_GIC_H

void gicc_init(unsigned long gicc_base);
void gicd_init(unsigned long gicd_base);
void gic_enable_interrupt(unsigned long gicd_base, unsigned int irq_id, unsigned int cpu_mask);
void gic_disable_interrupt(unsigned long gicd_base, unsigned int irq_id);

void enable_virtual_timer_irq(void);
void disable_virtual_timer_irq(void);

#endif
