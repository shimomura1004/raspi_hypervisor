#ifndef _GENERIC_TIMER_H
#define _GENERIC_TIMER_H

void timer_init(unsigned long irq_base_address);
// todo: 移動させる、たぶん clear_timer みたいな関数になる
void handle_timer_irq();

#endif  /*_GENERIC_TIMER_H */
