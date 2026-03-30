#include "log.h"
#include "psci.h"
#include "printf.h"

void print_boot_message() {
    // todo: 初期化関数を作ってそこに移す
    sm_log_init();
    init_printf(0, putc);
    
    printf("Secure monitor initialized\r\n");
}

void handle_smc(unsigned long x0, unsigned long x1, unsigned long x2, unsigned long x3, unsigned long x4) {
    switch (x0) {
        case PSCI_0_2_FN64_CPU_ON:
            printf("SMC: PSCI_CPU_ON\r\n");
            // todo: 実装する
            break;
        default:
            // todo: sm_print で printf フォーマットに対応する
            printf("SMC: Unknown Function ID 0x%08x\r\n", x0);
            break;
    }
}
