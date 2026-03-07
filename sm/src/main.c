#include "log.h"
#include "smccc.h"

void print_boot_message() {
    sm_print("Secure monitor initialized\r\n");
}

void handle_smc(unsigned long x0, unsigned long x1, unsigned long x2, unsigned long x3, unsigned long x4) {
    switch (x0) {
        case PSCI_0_2_FN64_CPU_ON:
            // todo: 出力が改行されない
            sm_print("SMC: PSCI_CPU_ON\r\n");
            // todo: 実装する
            break;
        default:
            // todo: sm_print で printf フォーマットに対応する
            sm_print("SMC: Unknown Function ID: ");
            break;
    }
}
