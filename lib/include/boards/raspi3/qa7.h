#ifndef _LIB_QA7
#define _LIB_QA7

void qa7_init(unsigned long qa7_base);
void qa7_enable_generic_timer(unsigned long qa7_base, unsigned long cpuid);
void qa7_enable_mailbox(unsigned long qa7_base, unsigned long cpuid);

void enable_virtual_timer_irq(void);
void disable_virtual_timer_irq(void);

#endif
