#ifndef _LIB_ARM_IRQ_H
#define _LIB_ARM_IRQ_H

#ifndef __ASSEMBLER__

void enable_irq(void);
void disable_irq(void);
int is_interrupt_enabled(void);

#endif

#endif /* _LIB_ARM_IRQ_H */
