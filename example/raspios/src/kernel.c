#include <stddef.h>
#include <stdint.h>

#include "printf.h"
#include "utils.h"
#include "generic_timer.h"
#include "irq.h"
#include "fork.h"
#include "sched.h"
#include "mini_uart.h"
#include "sys.h"
#include "user.h"
#include "spinlock.h"
#include "debug.h"

volatile unsigned long initialized = 0;
struct spinlock log_lock;

// ゲストから systimer にアクセスすると MMIO 領域にアクセスすることになるので、
// ホストの handle_mem_abort で処理される(処理の実体は bcm2837.c:handle_systimer_write にある)

void kernel_process(){
	INFO("Kernel process started. EL %d", get_el());
	unsigned long begin = (unsigned long)&user_begin;
	unsigned long end = (unsigned long)&user_end;
	unsigned long process = (unsigned long)&user_process;
	// ブートローダもしくは QEMU がカーネルの一部として
	// ユーザ空間用のコード(user_process)をメモリにロードしている
	// それをユーザ空間にコピーしている
	// PC は user_process の先頭にセット
	int err = move_to_user_mode(begin, end - begin, process - begin);
	if (err < 0){
		PANIC("Error while moving process to user mode\n\r");
	} 
}

void kernel_main()
{
	unsigned long cpuid = get_cpuid();
	// 初期タスク(アイドルタスク)の cpuid が -1 になっているので、現在の CPU ID を設定する
	currents[cpuid]->cpuid = cpuid;

	if (cpuid == 0) {
		// システムで一度だけ実施する初期化処理
		uart_init();
		init_printf(NULL, putc);

		init_lock(&log_lock, "log_lock");
		init_sched();

		INFO("raspios initialization complete");
	}

	// 各コアで実施する初期化処理
	irq_vector_init();
	timer_init();

	disable_irq();
	enable_interrupt_controller(cpuid);
	enable_irq();

	INFO("CPU %d started", cpuid);

	if (cpuid == 0) {
		int res = copy_process(PF_KTHREAD, (unsigned long)&kernel_process, 0);
		if (res < 0) {
			WARN("error while starting kernel process");
			return;
		}
		initialized = 1;
	}

	if (cpuid >= 4) {
		disable_interrupt_controller(cpuid);
		while (1) {
			INFO("CPU %d sleeps", cpuid);
			asm volatile("wfi");
		}
	}

	while (1){
		schedule();
		// ここ(idle プロセス)に返ってきたということはやることがないということなので
		// なんらかの割込みが発生するまでは CPU を休止させる
		asm volatile("wfi");
	}
}
