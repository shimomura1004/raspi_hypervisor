#include "utils.h"
#include "peripherals/pl011_uart_regs.h"
#include "drivers/uart.h"

#define BUF_SIZE 256

static unsigned long uart_base_address;
static char rx_buffer[BUF_SIZE];
static volatile int rx_head = 0;
static volatile int rx_tail = 0;

void uart_send(char c)
{
    // 送信バッファが空くまで待つビジーループ
    while (get32(uart_base_address + UART_FR_OFFSET) & UART_FR_TXFF)
        ;
    put32(uart_base_address + UART_DR_OFFSET, c);
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
    while (!(get32(uart_base_address + UART_FR_OFFSET) & UART_FR_RXFE)) {
        char c = get32(uart_base_address + UART_DR_OFFSET) & 0xFF;
        int next = (rx_head + 1) % BUF_SIZE;
        if (next != rx_tail) {
            rx_buffer[rx_head] = c;
            rx_head = next;
        }
    }
    // Clear the RX interrupt
    put32(uart_base_address + UART_ICR_OFFSET, UART_ICR_RXIC | UART_ICR_RTIC); 
}

void uart_init(unsigned long uart_base_virt_address, unsigned long gpio_base_virt_address)
{
    (void)gpio_base_virt_address; // PL011 では使用しないため明示的に無視
    uart_base_address = uart_base_virt_address;
    rx_head = rx_tail = 0;

    // まず UART を無効化
    put32(uart_base_address + UART_CR_OFFSET, 0);

    // 割込みをクリア
    put32(uart_base_address + UART_ICR_OFFSET, 0x7ff);

    // ボーレートの設定 (dummy values for QEMU)
    put32(uart_base_address + UART_IBRD_OFFSET, 1);
    put32(uart_base_address + UART_FBRD_OFFSET, 0);

    /* 8 bits, FIFO enabled, 1 stop bit, no parity */
    put32(uart_base_address + UART_LCRH_OFFSET, UART_LCRH_WLEN_8BIT | UART_LCRH_FEN);

    /* Enable RX and RX timeout interrupts */
    put32(uart_base_address + UART_IMSC_OFFSET, UART_IMSC_RXIM | UART_IMSC_RTIM);

    /* Enable UART, TX, RX */
    put32(uart_base_address + UART_CR_OFFSET, UART_CR_UARTEN | UART_CR_TXE | UART_CR_RXE);
}

void putc(void* p, char c)
{
    if (c == '\n') {
        uart_send('\r');
    }
    uart_send(c);
}
