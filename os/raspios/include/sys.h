#ifndef	_SYS_H
#define	_SYS_H

#define __NR_syscalls	 4

#ifndef __ASSEMBLER__

void sys_write(char * buf);
int sys_fork();
void sys_set_priority(long priority);

#endif

#endif  /*_SYS_H */
