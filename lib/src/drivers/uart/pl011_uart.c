#include "utils.h"
#include "peripherals/pl011_uart.h"
#include "drivers/uart.h"

#define BUF_SIZE 256
static char rx_buffer[BUF_SIZE];
static volatile int rx_head = 0;
static volatile int rx_tail = 0;

void uart_send(char c)
{
    // 送信バッファが空くまで待つビジーループ
	while (get32(UART_FR) & UART_FR_TXFF)
		;
	put32(UART_DR, c);
}

char uart_recv(void)
{
	while (1) {
		if (rx_head != rx_tail) {
			break;
		}
		// Wait for interrupt
		asm volatile("wfi");
	}
	char c = rx_buffer[rx_tail];
	rx_tail = (rx_tail + 1) % BUF_SIZE;
	return c;
}

void handle_uart_irq(void)
{
	while (!(get32(UART_FR) & UART_FR_RXFE)) {
		char c = get32(UART_DR) & 0xFF;
		int next = (rx_head + 1) % BUF_SIZE;
		if (next != rx_tail) {
			rx_buffer[rx_head] = c;
			rx_head = next;
		}
	}
	// Clear the RX interrupt
	put32(UART_ICR, (1 << 4)); 
}

void uart_init(void)
{
    // まず UART を無効化
	put32(UART_CR, 0);

    // 割込みをクリア
	put32(UART_ICR, 0x7ff);

    // ボーレートの設定 (dummy values for QEMU)
	put32(UART_IBRD, 1);
	put32(UART_FBRD, 0);

	/* 8 bits, FIFO enabled, 1 stop bit, no parity */
	put32(UART_LCRH, (3 << 5) | (1 << 4));

	/* Enable RX interrupt */
	put32(UART_IMSC, (1 << 4)); // RXIM

	/* Enable UART, TX, RX */
	put32(UART_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

void putc(void* p, char c)
{
	uart_send(c);
}
