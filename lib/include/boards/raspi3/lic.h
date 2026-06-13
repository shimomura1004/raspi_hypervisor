#ifndef _LIB_LIC
#define _LIB_LIC

void lic_enable_aux(unsigned long irq_base);
void lic_disable_aux(unsigned long irq_base);
void lic_enable_mbox(unsigned long irq_base);
void lic_enable_systimer(unsigned long irq_base);

#endif
