#ifndef _LIB_ARM_IRQ_H
#define _LIB_ARM_IRQ_H

#ifndef __ASSEMBLER__

void unmask_irq(void);
void mask_irq(void);
int is_irq_unmasked(void);

#endif

#endif /* _LIB_ARM_IRQ_H */
