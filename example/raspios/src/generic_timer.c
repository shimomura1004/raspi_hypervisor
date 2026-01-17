#include "utils.h"
#include "sched.h"
#include "debug.h"

// todo: 切り替えが多発しているように見えていたが、実際にはまったく切り替わっていなかった
//       マルチコアで複数プロセスが動いていたため切り替わっているように見えていた

// todo: 割込みの発生が指定した時間通りになっていない
const unsigned int interval = 200000; // 20ms

void timer_init ( void )
{
	unsigned long frequency;
	asm volatile("mrs %0, cntfrq_el0" : "=r"(frequency));
	unsigned long ticks = (frequency * interval) / 1000 / 1000;
	INFO("Timer frequency: %u MHz, ticks: %u", frequency / 1000 / 1000, ticks);
	asm volatile("msr cntv_tval_el0, %0" : : "r"(ticks));
	// enable virtual timer interrupt
	asm volatile("msr cntv_ctl_el0, %0" : : "r"(1));
}

void handle_timer_irq( void ) 
{
	unsigned long frequency;
	asm volatile("mrs %0, cntfrq_el0" : "=r"(frequency));
	unsigned long ticks = (frequency * interval) / 1000 / 1000;
	asm volatile("msr cntv_tval_el0, %0" : : "r"(ticks));
	timer_tick();
}
