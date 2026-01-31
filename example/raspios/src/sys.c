#include "fork.h"
#include "printf.h"
#include "mini_uart.h"

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

void * const sys_call_table[] = {sys_write, sys_fork, sys_exit};
