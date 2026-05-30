#ifndef _RASPIOS_TIMER_H
#define _RASPIOS_TIMER_H

void raspios_timer_init(unsigned long irq_base_virt_address, unsigned int timer_interval_ms);
void raspios_handle_timer_irq();

#endif