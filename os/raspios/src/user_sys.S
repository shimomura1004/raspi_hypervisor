.section ".text.user"

.set SYS_WRITE_NUMBER, 0 		// syscal numbers 
.set SYS_FORK_NUMBER, 1 	
.set SYS_EXIT_NUMBER, 2 	
.set SYS_SET_PRIORITY_NUMBER, 3 	

.globl user_delay
user_delay:
	subs x0, x0, #1
	bne user_delay
	ret

.globl call_sys_write
call_sys_write:
	mov w8, #SYS_WRITE_NUMBER
	svc #0
	ret

.globl call_sys_exit
call_sys_exit:
	mov w8, #SYS_EXIT_NUMBER
	svc #0
	ret

.globl call_sys_fork
call_sys_fork:
	mov w8, #SYS_FORK_NUMBER
	svc #0
	ret

.globl call_sys_set_priority
call_sys_set_priority:
	mov w8, #SYS_SET_PRIORITY_NUMBER
	svc #0
	ret
