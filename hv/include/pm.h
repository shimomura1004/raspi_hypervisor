#ifndef _PM_H
#define _PM_H

void system_shutdown(void);
void system_reboot(void);
int is_system_halted(void);
void halt_current_cpu(void);

#endif
