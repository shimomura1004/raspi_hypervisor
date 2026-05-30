#ifndef _GENERIC_TIMER_H
#define _GENERIC_TIMER_H

void enable_virtual_timer_interrupt();
void set_next_tick(unsigned int interval_ms);

void enable_virtual_timer_interrupt_el2();
void set_next_tick_el2(unsigned int interval_ms);

#endif  /*_GENERIC_TIMER_H */
