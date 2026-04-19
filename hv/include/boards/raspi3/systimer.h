#ifndef _BOARDS_RASPI3_SYSTIMER_H
#define _BOARDS_RASPI3_SYSTIMER_H

void systimer_init(void);
unsigned long get_physical_systimer_count(void);
void show_systimer_info(void);

#endif  /* _BOARDS_RASPI3_SYSTIMER_H */
