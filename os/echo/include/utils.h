#ifndef	_BOOT_H
#define	_BOOT_H

#include "common_utils.h"

extern void issue_hvc(unsigned long);
extern void issue_smc(unsigned long);

#endif  /*_BOOT_H */
