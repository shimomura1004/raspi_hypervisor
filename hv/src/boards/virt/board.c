#include "board.h"
#include <stddef.h>

static void virt_initialize(struct vm_struct2 *vm) {
    // todo: 実装する
}

static int virt_is_irq_asserted(struct vcpu_struct *vcpu) {
    // todo: 実装する
    return 0;
}

const struct board_ops virt_board_ops = {
    .initialize = virt_initialize,
    .mmio_read = NULL,
    .mmio_write = NULL,
    .entering_vm = NULL,
    .leaving_vm = NULL,
    .is_irq_asserted = virt_is_irq_asserted,
    .is_fiq_asserted = NULL,
    .debug = NULL,
};

const struct board_ops *get_default_board_ops(void) {
    return &virt_board_ops;
}
