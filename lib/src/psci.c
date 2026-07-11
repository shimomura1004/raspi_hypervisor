#include "psci.h"
#include "smccc.h"

int psci_cpu_on_hvc(uint64_t target_cpu, uint64_t entry_point, uint64_t context_id) {
    return (int)hvc_call_4(PSCI_0_2_FN64_CPU_ON, target_cpu, entry_point, context_id);
}

int psci_cpu_on_smc(uint64_t target_cpu, uint64_t entry_point, uint64_t context_id) {
    return (int)smc_call_4(PSCI_0_2_FN64_CPU_ON, target_cpu, entry_point, context_id);
}

int psci_system_off_smc(void) {
    smc_call_4(PSCI_0_2_FN_SYSTEM_OFF, 0, 0, 0);
    return -1;
}

int psci_system_reset_smc(void) {
    smc_call_4(PSCI_0_2_FN_SYSTEM_RESET, 0, 0, 0);
    return -1;
}