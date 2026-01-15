#include "utils.h"
#include "printf.h"
#include "sched.h"

const unsigned int interval = 20000; // 20ms

void timer_init ( void )
{
	unsigned long frequency;
	asm volatile("mrs %0, cntfrq_el0" : "=r"(frequency));
	unsigned long ticks = (frequency * interval) / 1000000;
	asm volatile("msr cntv_tval_el0, %0" : : "r"(ticks));
	// enable virtual timer interrupt
	asm volatile("msr cntv_ctl_el0, %0" : : "r"(1));
}

void handle_timer_irq( void ) 
{
	unsigned long frequency;
	asm volatile("mrs %0, cntfrq_el0" : "=r"(frequency));
	unsigned long ticks = (frequency * interval) / 1000000;
	asm volatile("msr cntv_tval_el0, %0" : : "r"(ticks));
	timer_tick();
}
