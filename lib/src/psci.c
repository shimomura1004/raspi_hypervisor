#include "psci.h"
#include "smccc.h"

// todo: ゲスト OS で動く場合は pCPU を有効にしてはいけない、代わりに vCPU を有効にする
// qemu で OS をネイティブに動かす場合は qemu の hv がいい感じにやってくれる
int psci_cpu_on_hvc(uint64_t target_cpu, uint64_t entry_point, uint64_t context_id) {
    return (int)hvc_call_4(PSCI_0_2_FN64_CPU_ON, target_cpu, entry_point, context_id);
}

int psci_cpu_on_smc(uint64_t target_cpu, uint64_t entry_point, uint64_t context_id) {
    return (int)smc_call_4(PSCI_0_2_FN64_CPU_ON, target_cpu, entry_point, context_id);
}

#if defined(BOARD_VIRT)
int psci_system_off_smc(void) {
    smc_call_4(PSCI_0_2_FN_SYSTEM_OFF, 0, 0, 0);
    // ここには帰ってこないはずなので、帰ってきた場合はエラー
    return -1;
}

int psci_system_reset_smc(void) {
    smc_call_4(PSCI_0_2_FN_SYSTEM_RESET, 0, 0, 0);
    // ここには帰ってこないはずなので、帰ってきた場合はエラー
    return -1;
}
#endif
