#ifndef _EXCEPTION_H
#define _EXCEPTION_H

void show_invalid_entry_message(int type, unsigned long esr, unsigned long elr, unsigned long far, unsigned long mpidr);

#endif /* _EXCEPTION_H */
