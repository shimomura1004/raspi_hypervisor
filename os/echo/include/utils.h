#ifndef	_BOOT_H
#define	_BOOT_H

extern void delay ( unsigned long);
extern void put32 ( unsigned long, unsigned int );
extern unsigned int get32 ( unsigned long );
extern int get_el ( void );
extern void issue_hvc(unsigned long);
extern void issue_smc(unsigned long);

#endif  /*_BOOT_H */
