#include "console.h"
#include "vm.h"
#include "sched.h"
#include "fifo.h"
#include "utils.h"
#include "debug.h"
#include "systimer.h"

// このハイパーバイザのエスケープ文字
#define ESCAPE_CHAR '?'

// UART から入力されたデータを送り込む先の VM の番号(0 のときはホスト)
// todo: ホスト用にたまったデータはいつ出力されている？
static int uart_forwarded_vm = 0;

int is_uart_forwarded_vm(struct vm_struct2 *vm) {
    return vm->vmid == uart_forwarded_vm;
}

void switch_console_to_vm(int vmid) {
    uart_forwarded_vm = vmid;

    struct vm_struct2 *vm = vms2[uart_forwarded_vm];
    if (vm) {
        flush_vm_console(vm);
    }
}

// uart_forwarded_vm が指す VM かホストに文字データを追加する
// todo: キューに値が入っているときは、そのゲストに切り替わったときに仮想割り込みを発生させる
void console_handle_char(char received) {
    static int is_escaped = 0;
    struct vm_struct2 *vm;

    if (is_escaped) {
        is_escaped = 0;

        if (isdigit(received)) {
            // VM を切り替えるのではなく、単に UART 入力の送り先を変えるだけ
            // todo: ここはロックしなくて大丈夫？
            switch_console_to_vm(received - '0');
            INFO("Console is now used by VM %d", uart_forwarded_vm);
        }
        else if (received == 'l') {
            show_vm_list();
        }
        else if (received == 't') {
#if defined(BOARD_RASPI3)
            show_systimer_info();
#endif
        }
        else if (received == ESCAPE_CHAR) {
            goto enqueue_char;
        }
    }
    else if (received == ESCAPE_CHAR) {
        is_escaped = 1;
    }
    else {
enqueue_char:
        vm = vms2[uart_forwarded_vm];
        // もし VM が動いていたらキューに入れておく
        if (vm) {
            enqueue_fifo(vm->console.in_fifo, received);
        }
    }
}
