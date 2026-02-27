#include "user_sys.h"
#include "user.h"
#include "printf.h"

void loop(char* str, int length)
{
	char buf[2] = {""};
	while (1){
		for (int i = 0; i < length; i++){
			buf[0] = str[i];
			call_sys_write(buf);
			// todo: ビジーループをやめて sleep を使いたい
			user_delay(1000000);
		}
	}
}

void user_process() 
{
	call_sys_write("User process\n\r");
	int pid = call_sys_fork();
	if (pid < 0) {
		call_sys_write("Error during fork\n\r");
		call_sys_exit();
		return;
	}
	if (pid == 0){
		call_sys_set_priority(3);
		call_sys_write("Child process started 0\n\r");
		loop("*", 1);
	} else {
		int pid = call_sys_fork();
		if (pid < 0) {
			call_sys_write("Error during fork\n\r");
			call_sys_exit();
			return;
		}
		if (pid == 0) {
			call_sys_set_priority(2);
			call_sys_write("Child process started 1\n\r");
			loop("+", 1);
		}
		else {
			call_sys_set_priority(1);
			call_sys_write("Child process started 2\n\r");
			loop("-", 1);
		}
	}
}

