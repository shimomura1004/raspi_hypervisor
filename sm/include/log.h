#ifndef SM_LOG_H
#define SM_LOG_H

void sm_log_init(void);
void sm_putc(char c);
void sm_print(const char *s);
void putc(void* p, char c);

#endif
