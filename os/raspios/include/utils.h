#ifndef	_UTILS_H
#define	_UTILS_H

#include "common_utils.h"

extern void delay ( unsigned long);
extern unsigned long get_el ( void );
extern unsigned long get_cpuid();
extern void set_pgd(unsigned long pgd);
extern unsigned long get_pgd();

#endif  /*_UTILS_H */
