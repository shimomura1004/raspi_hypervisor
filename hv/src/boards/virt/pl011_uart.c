#include "board_config.h"
#include "peripherals/pl011_uart.h"
#include "console.h"
#include "utils.h"
#include "mm.h"

struct spinlock console_lock;

static void _uart_send(char c) {
    // 送信バッファが空くまで待つビジーループ
    while (get32(P2V(UART_FR)) & UART_FR_TXFF)
        ;
    put32(P2V(UART_DR), c);
}

void handle_console_irq(void) {
    // todo: 実装する
}

void console_init(void) {
    init_lock(&console_lock, "console");

    // まず UART を無効化
    put32(P2V(UART_CR), 0);
    
    // 割込みをクリア
    put32(P2V(UART_ICR), 0x7ff);

    // ボーレートの設定 (dummy values for QEMU)
    put32(P2V(UART_IBRD), 1);
    put32(P2V(UART_FBRD), 0);
    
    /* 8 bits, FIFO enabled, 1 stop bit, no parity */
    put32(P2V(UART_LCRH), (3 << 5) | (1 << 4));
    
    // UART の割込みを有効化
    put32(P2V(UART_CR), (1 << 0) | (1 << 8) | (1 << 9));
}

void putc(void *p, char c) {
    if (c == '\n') {
        _uart_send('\r');
    }
    _uart_send(c);
}
