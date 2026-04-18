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
    // 受信キューが空ではない間繰り返す
    while (!(get32(P2V(UART_FR)) & UART_FR_RXFE)) {
        char c = get32(P2V(UART_DR)) & 0xFF;
        console_handle_char(c);
    }
    put32(P2V(UART_ICR), UART_ICR_RXIC);
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
    put32(P2V(UART_LCRH), UART_LCRH_WLEN_8BIT | UART_LCRH_FEN);

    /* Enable RX interrupt */
    put32(P2V(UART_IMSC), UART_IMSC_RXIM);
    
    // UART の割込みを有効化
    put32(P2V(UART_CR), UART_CR_UARTEN | UART_CR_TXE | UART_CR_RXE);
}

void putc(void *p, char c) {
    if (c == '\n') {
        _uart_send('\r');
    }
    _uart_send(c);
}
