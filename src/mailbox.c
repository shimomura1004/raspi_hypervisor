#include "peripherals/mailbox.h"
#include "sched.h"
#include "pm.h"
#include "debug.h"

// todo: アクセサを使う
extern volatile int system_halted;

void handle_mailbox_irq(unsigned long cpuid) {
    // 現状は、メールボックスへの通知は pCPU の停止指示を意味する
    if (system_halted) {
        halt_current_cpu();
    }
}
