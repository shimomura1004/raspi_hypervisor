#include <stddef.h>
#include <stdint.h>
#include "board.h"
#include "mm.h"
#include "console.h"
#include "fifo.h"
#include "peripherals/pl011_uart.h"

// todo: 0x1000 を定数化する
#define ADDR_IN_AUX(a)      (UART_BASE <= (a) && (a) < UART_BASE + 0x1000)

struct virt_board_state {
    unsigned long uart_imsc;
};

static void virt_initialize(struct vm_struct2 *vm) {
    struct virt_board_state *state = (struct virt_board_state *)allocate_page();
    state->uart_imsc = 0;
    vm->board_data = state;

    // todo: その他の初期化を行う

    // stage2 のデバイスのメモリマッピング(MMIO ページの準備)
    // アクセスをトラップするためにアクセス不可に設定する
    unsigned long begin = DEVICE_BASE;
    // unsigned long end = DEVICE_BASE + DEVICE_SIZE;
    unsigned long end = RAM_BASE;
    for (; begin < end; begin += PAGE_SIZE) {
        set_vm_page_notaccessable(vm, begin);
    }
}

static unsigned long handle_aux_read(struct vcpu_struct *vcpu, unsigned long addr) {
    struct virt_board_state *state = (struct virt_board_state *)vcpu->vm->board_data;

    switch (addr) {
    case UART_FR:
        // todo: ちゃんと実装する
        // 常に送信可能(TXFF=0)、受信データなし(RXFE=1)として返す
        return UART_FR_RXFE;
    case UART_DR: {
        unsigned long data = 0;
        dequeue_fifo(vcpu->vm->console.in_fifo, &data);
        return data & 0xff;
    }
    }

    return 0;
}

static void handle_aux_write(struct vcpu_struct *vcpu, unsigned long addr, unsigned long val) {
    struct virt_board_state *state = (struct virt_board_state *)vcpu->vm->board_data;

    // todo: 他のレジスタへの書き込みも対応する
    switch (addr) {
    case UART_DR:
        if (is_uart_forwarded_vm(vcpu->vm)) {
            // アクティブな VM なら直接出力
            acquire_lock(&console_lock);
            putc(NULL, val & 0xff);
            release_lock(&console_lock);
        } else {
            // 非アクティブならバッファに貯める
            enqueue_fifo(vcpu->vm->console.out_fifo, val & 0xff);
        }
        break;
    default:
        // 他のレジスタ(LCRH, CR 等)への書き込みは今のところ無視
        break;
    }
}

static unsigned long virt_mmio_read(struct vcpu_struct *vcpu, unsigned long addr) {
    if (ADDR_IN_AUX(addr)) {
        return handle_aux_read(vcpu, addr);
    }

    return 0;
}

static void virt_mmio_write(struct vcpu_struct *vcpu, uint64_t addr, uint64_t value) {
    // todo: 他の領域への書き込みも対応
    if (ADDR_IN_AUX(addr)) {
        return handle_aux_write(vcpu, addr, value);
    }
}

static int virt_is_irq_asserted(struct vcpu_struct *vcpu) {
    struct virt_board_state *state = (struct virt_board_state *)vcpu->vm->board_data;
    if (!state) {
    return 0;
    }
}    

const struct board_ops virt_board_ops = {
    .initialize = virt_initialize,
    .mmio_read = virt_mmio_read,
    .mmio_write = virt_mmio_write,
    .entering_vm = NULL,
    .leaving_vm = NULL,
    .is_irq_asserted = virt_is_irq_asserted,
    .is_fiq_asserted = NULL,
    .debug = NULL,
};

const struct board_ops *get_default_board_ops(void) {
    return &virt_board_ops;
}
