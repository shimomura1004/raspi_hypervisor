#ifndef _PSCI_H
#define _PSCI_H

// todo: lib に移動するべきかも

#include <stdint.h>

#define PSCI_0_2_FN64_BASE      0xC4000000
#define PSCI_0_2_FN64_CPU_ON    (PSCI_0_2_FN64_BASE + 3)

int psci_cpu_on(uint64_t target_cpu, uint64_t entry_point, uint64_t context_id);

#endif /* _PSCI_H */
