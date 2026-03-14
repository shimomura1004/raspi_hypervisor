#include "utils.h"
#include "sched.h"
#include "peripherals/irq.h"
#include "debug.h"

static const unsigned int interval_ms = 200;

void timer_init ( void )
{
	unsigned long frequency;
	asm volatile("mrs %0, cntfrq_el0" : "=r"(frequency));
	unsigned long ticks = (frequency * interval_ms) / 1000;
	INFO("Timer frequency: %u MHz, ticks: %u", frequency / 1000 / 1000, ticks);
	asm volatile("msr cntv_tval_el0, %0" : : "r"(ticks));
	// enable virtual timer interrupt
	asm volatile("msr cntv_ctl_el0, %0" : : "r"(1));
}

void handle_timer_irq( void ) 
{
	unsigned long frequency;
	asm volatile("mrs %0, cntfrq_el0" : "=r"(frequency));
	unsigned long ticks = (frequency * interval_ms) / 1000;
	asm volatile("msr cntv_tval_el0, %0" : : "r"(ticks));

	// todo: これは raspi3 のシステムタイマを前提にしたときのみ必要なもの？
	//       raspi3 ボードをエミュレートするときのみ実行されるようにするべき
	clear_timer_irq();

	timer_tick();
}
