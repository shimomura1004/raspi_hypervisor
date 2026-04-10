#include "fork.h"
#include "printf.h"
#include "drivers/uart.h"
#include "utils.h"
#include "sched.h"
#include "debug.h"

void sys_write(char * buf){
	acquire_lock(&console_lock);
	printf(buf);
	release_lock(&console_lock);
}

int sys_fork(){
	return copy_process(0, 0, 0);
}

void sys_exit(){
	exit_process();
}

void sys_set_priority(long priority){
	int cpuid = get_cpuid();
	currents[cpuid]->priority = priority;
	// 次のタイムスライス更新時に反映されるよう counter をリセットする
	currents[cpuid]->counter = priority;
}

void * const sys_call_table[] = {sys_write, sys_fork, sys_exit, sys_set_priority};
