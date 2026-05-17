#ifndef _BOARDS_RASPI3_PERIPHERALS_SYSTIMER_H
#define _BOARDS_RASPI3_PERIPHERALS_SYSTIMER_H

void systimer_init(unsigned long base_virt_address, unsigned long systimer_interval);
unsigned long get_physical_systimer_count(void);
// todo: これは hv に移す
void show_systimer_info(void);

#endif  /* _BOARDS_RASPI3_PERIPHERALS_SYSTIMER_H */
