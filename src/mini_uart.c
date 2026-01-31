#include "peripherals/mini_uart.h"
#include "peripherals/gpio.h"
#include "printf.h"
#include "utils.h"
#include "sched.h"
#include "fifo.h"
#include "vm.h"
#include "systimer.h"
#include "debug.h"

static void _uart_send(char c) {
    // 送信バッファが空くまで待つビジーループ
    while (1) {
        if (get32(AUX_MU_LSR_REG) & 0x20) {
            break;
        }
    }
    put32(AUX_MU_IO_REG, c);
}

// static void uart_send(char c) {
//     if (c == '\n' || c == '\r') {
//         _uart_send('\r');
//         _uart_send('\n');
//     } else {
//         _uart_send(c);
//     }
// }

// static char uart_recv(void) {
//     // 受信バッファにデータが届くまで待つビジーループ
//     while (1) {
//         if (get32(AUX_MU_LSR_REG) & 0x01) {
//             break;
//         }
//     }

// 	char c = get32(AUX_MU_IO_REG) & 0xFF;
// 	if (c == '\r') {
// 		return '\n';
// 	}
//     return c;
// }

// static void uart_send_string(char *str) {
//     for (int i = 0; str[i] != '\0'; i++) {
//         _uart_send((char)str[i]);
//     }
// }

// このハイパーバイザのエスケープ文字
#define ESCAPE_CHAR '?'

// UART から入力されたデータを送り込む先の VM の番号(0 のときはホスト)
// todo: ホスト用にたまったデータはいつ出力されている？
static int uart_forwarded_vm = 0;

int is_uart_forwarded_vm(struct vm_struct2 *vm) {
    return vm->vmid == uart_forwarded_vm;
}

// todo: 送信バッファが空、受信バッファにデータあり、といった割込みを受けて wakeup させる
static void handle_uart_irq_send(void) {
    INFO("SEND!");
}

// uart_forwarded_vm が指す VM かホストに文字データを追加する
// todo: キューに値が入っているときは、そのゲストに切り替わったときに仮想割り込みを発生させる
static void handle_uart_irq_recv(void) {
    static int is_escaped = 0;

    char received = get32(AUX_MU_IO_REG) & AUX_MU_IO_REG_DATA;
    struct vm_struct2 *vm;

    if (is_escaped) {
        is_escaped = 0;

        if (isdigit(received)) {
            // VM を切り替えるのではなく、単に UART 入力の送り先を変えるだけ
            // todo: ここはロックしなくて大丈夫？
            uart_forwarded_vm = received - '0';
            INFO("Console is now used by VM %d", uart_forwarded_vm);
            vm = vms2[uart_forwarded_vm];
            if (vm) {
                flush_vm_console(vm);
            }
        }
        else if (received == 'l') {
            show_vm_list();
        }
        else if (received == 't') {
            show_systimer_info();
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

void handle_uart_irq(void) {
    unsigned int iir = get32(AUX_MU_IIR_REG);
    unsigned int iir_interrupt_id = (iir & AUX_MU_IIR_REG_INTERRUPT_ID) >> 1;

    if (iir_interrupt_id == 0b01) {
        // 送信バッファが空
        handle_uart_irq_send();
    }
    else if (iir_interrupt_id == 0b10) {
        // 受信バッファにデータあり
        handle_uart_irq_recv();
    }
}

void uart_init(void) {
    unsigned int selector;

    selector = get32(GPFSEL1);
    selector &= ~(7 << 12); // clean gpio14
    // 2(0b010) は alt5 を選ぶという意味
    selector |= 2 << 12;    // set alt5 for gpio14
    selector &= ~(7 << 15); // clean gpio15
    selector |= 2 << 15;    // set alt5 for gpio15
    put32(GPFSEL1, selector);

    // GPIO のpull-up/pull-down の設定をクリアする
    put32(GPPUD, 0);
    delay(150);
    put32(GPPUDCLK0, (1 << 14) | (1 << 15));
    delay(150);
    put32(GPPUDCLK0, 0);

    // UART としての設定を行う

    // Enable mini uart
    // (this also enables access to its registers)
    put32(AUX_ENABLES, 1);

    // Disable auto flow control and disable receiver and transmitter (for now)
    put32(AUX_MU_CNTL_REG, 0);

    // Enable transmit/receive interrupts
    put32(AUX_MU_IER_REG, AUX_MU_IER_REG_ENABLE_RECEIVE_INTERRUPTS);

    // Enable 8 bit mode
    put32(AUX_MU_LCR_REG, 3);

    // Set RTS line to be always high
    put32(AUX_MU_MCR_REG, 0);
    // Set baud rate to 115200
    put32(AUX_MU_BAUD_REG, 270);

    // Finally, enable transmitter and receiver
    put32(AUX_MU_CNTL_REG, 3);
}

// This function is required by printf function
void putc(void *p, char c) { _uart_send(c); }
