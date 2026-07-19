#ifndef _BOOT_H
#define _BOOT_H

#include "common_utils.h"

extern void issue_hvc(uint64_t, uint64_t, uint64_t, uint64_t);
extern void issue_smc(uint64_t, uint64_t, uint64_t, uint64_t);

#endif  /*_BOOT_H */
