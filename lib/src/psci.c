#include "psci.h"
#include "smccc.h"

int psci_cpu_on_hvc(uint64_t target_cpu, uint64_t entry_point, uint64_t context_id) {
    return (int)hvc_call_4(PSCI_0_2_FN64_CPU_ON, target_cpu, entry_point, context_id);
}

int psci_cpu_on_smc(uint64_t target_cpu, uint64_t entry_point, uint64_t context_id) {
    // 現状 lib に smc_call_4 の実装が必要ですが、インターフェースとしては共通化可能です
    return (int)smc_call_4(PSCI_0_2_FN64_CPU_ON, target_cpu, entry_point, context_id);
}