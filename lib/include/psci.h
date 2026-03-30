#ifndef _PSCI_H
#define _PSCI_H

#include <stdint.h>
#include "smccc.h"

// ARM PSCI Function IDs (Example)
#define PSCI_0_2_FN_PSCI_VERSION        0x84000000
#define PSCI_0_2_FN_SYSTEM_OFF          0x84000008
#define PSCI_0_2_FN_SYSTEM_RESET        0x84000009

// Vendor Specific Hypervisor Service Calls (Range 6)
// SMC64 を想定する場合は 0xC6... を使用する
#define VENDOR_HYP_RANGE_START          0x86000000
#define VENDOR_HYP_RANGE_END            0x8600FFFF

#define PSCI_0_2_FN64_BASE              0xC4000000
#define PSCI_0_2_FN64_CPU_ON            0xC4000003

int psci_cpu_on_hvc(uint64_t target_cpu, uint64_t entry_point, uint64_t context_id);
int psci_cpu_on_smc(uint64_t target_cpu, uint64_t entry_point, uint64_t context_id);

#endif /* _PSCI_H */
