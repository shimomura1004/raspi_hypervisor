#include "psci.h"

int psci_cpu_on(uint64_t target_cpu, uint64_t entry_point, uint64_t context_id) {
    register uint64_t x0 asm("x0") = PSCI_0_2_FN64_CPU_ON;
    register uint64_t x1 asm("x1") = target_cpu;
    register uint64_t x2 asm("x2") = entry_point;
    register uint64_t x3 asm("x3") = context_id;

    // virt ボード上のハイパーバイザまたは QEMU に対して HVC 命令を発行
    asm volatile(
        "hvc #0\n"
        : "+r"(x0)
        : "r"(x1), "r"(x2), "r"(x3)
        : "memory"
    );

    return (int)x0;
}
