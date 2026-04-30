#include "console.h"
#include "mm.h"
#include "peripherals/mini_uart_regs.h"
#include "peripherals/gpio.h"
#include "printf.h"
#include "utils.h"
#include "sched.h"
#include "fifo.h"
#include "vm.h"
#include "systimer.h"
#include "debug.h"
#include "spinlock.h"

struct spinlock uart_lock;
struct spinlock console_lock;

static void _uart_send(char c) {
    acquire_lock(&uart_lock);
    // 送信バッファが空くまで待つビジーループ
    while (1) {
        if (get32(P2V(AUX_MU_LSR_REG)) & 0x20) {
            break;
        }
    }
    put32(P2V(AUX_MU_IO_REG), c);
    release_lock(&uart_lock);
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
//         if (get32(P2V(AUX_MU_LSR_REG)) & 0x01) {
//             break;
//         }
//     }

// 	char c = get32(P2V(AUX_MU_IO_REG)) & 0xFF;
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



// todo: 送信バッファが空、受信バッファにデータあり、といった割込みを受けて wakeup させる
static void handle_uart_irq_send(void) {
    INFO("SEND!");
}

static void handle_uart_irq_recv(void) {
    char received = get32(P2V(AUX_MU_IO_REG)) & AUX_MU_IO_REG_DATA;
    console_handle_char(received);
}

void handle_console_irq(void) {
    unsigned int iir = get32(P2V(AUX_MU_IIR_REG));
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

void console_init(void) {
    init_lock(&uart_lock, "uart");
    init_lock(&console_lock, "console");

    unsigned int selector;

    selector = get32(P2V(GPFSEL1));
    selector &= ~(7 << 12); // clean gpio14
    // 2(0b010) は alt5 を選ぶという意味
    selector |= 2 << 12;    // set alt5 for gpio14
    selector &= ~(7 << 15); // clean gpio15
    selector |= 2 << 15;    // set alt5 for gpio15
    put32(P2V(GPFSEL1), selector);

    // GPIO のpull-up/pull-down の設定をクリアする
    put32(P2V(GPPUD), 0);
    delay(150);
    put32(P2V(GPPUDCLK0), (1 << 14) | (1 << 15));
    delay(150);
    put32(P2V(GPPUDCLK0), 0);

    // UART としての設定を行う

    // Enable mini uart
    // (this also enables access to its registers)
    put32(P2V(AUX_ENABLES), 1);

    // Disable auto flow control and disable receiver and transmitter (for now)
    put32(P2V(AUX_MU_CNTL_REG), 0);

    // Enable transmit/receive interrupts
    put32(P2V(AUX_MU_IER_REG), AUX_MU_IER_REG_ENABLE_RECEIVE_INTERRUPTS);

    // Enable 8 bit mode
    put32(P2V(AUX_MU_LCR_REG), 3);

    // Set RTS line to be always high
    put32(P2V(AUX_MU_MCR_REG), 0);
    // Set baud rate to 115200
    put32(P2V(AUX_MU_BAUD_REG), 270);

    // Finally, enable transmitter and receiver
    put32(P2V(AUX_MU_CNTL_REG), 3);
}

// This function is required by printf function
void putc(void *p, char c) { _uart_send(c); }
