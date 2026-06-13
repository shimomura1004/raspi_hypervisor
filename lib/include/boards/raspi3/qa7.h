#ifndef _LIB_QA7
#define _LIB_QA7

void init_qa7(unsigned long qa7_virt_base);

void arm_local_timer_enable(unsigned long cpuid);
void arm_local_ipi_enable(unsigned long cpuid);

void enable_virtual_timer_irq(void);
void disable_virtual_timer_irq(void);

#endif
