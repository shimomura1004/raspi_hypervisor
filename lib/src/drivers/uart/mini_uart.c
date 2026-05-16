#include "utils.h"
#include "board_config.h"
#include "drivers/uart.h"

#define BUF_SIZE 256

static unsigned long uart_base_address;
static unsigned long gpio_base_address;
static char rx_buffer[BUF_SIZE];
static volatile int rx_head = 0;
static volatile int rx_tail = 0;

void uart_send(char c)
{
    while(1) {
        if(get32(uart_base_address + AUX_MU_LSR_REG_OFFSET) & 0x20) {
            break;
        }
    }
    put32(uart_base_address + AUX_MU_IO_REG_OFFSET, c);
}

char uart_recv(void)
{
    while(1) {
        if(rx_head != rx_tail) {
            break;
        }

        // UART を受信して割込みが発生するまでは CPU 停止
        // UART 以外の割込みでも復帰するので while 文でガードする
        asm volatile("wfi");
    }
    char c = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % BUF_SIZE;
    return c;
}

void handle_uart_irq(void)
{
    while(get32(uart_base_address + AUX_MU_LSR_REG_OFFSET) & 0x01) {
        char c = get32(uart_base_address + AUX_MU_IO_REG_OFFSET) & 0xFF;
        int next = (rx_head + 1) % BUF_SIZE;
        if (next != rx_tail) {
            rx_buffer[rx_head] = c;
            rx_head = next;
        }
    }
}

void uart_init(unsigned long uart_base_virt_address, unsigned long gpio_base_virt_address)
{
    unsigned int selector;
    uart_base_address = uart_base_virt_address;
    gpio_base_address = gpio_base_virt_address;

    // raspi3 では UART を使うために GPIO が必要
    selector = get32(gpio_base_address + GPFSEL1_OFFSET);
    selector &= ~(7 << 12);         // clean gpio14
    selector |= 2 << 12;            // set alt5 for gpio14
    selector &= ~(7 << 15);         // clean gpio15
    selector |= 2 << 15;            // set alt5 for gpio15
    put32(gpio_base_address + GPFSEL1_OFFSET, selector);

    put32(gpio_base_address + GPPUD_OFFSET, 0);
    delay(150);
    put32(gpio_base_address + GPPUDCLK0_OFFSET, (1 << 14) | (1 << 15));
    delay(150);
    put32(gpio_base_address + GPPUDCLK0_OFFSET, 0);

    put32(uart_base_address + AUX_ENABLES_OFFSET, 1);        // Enable mini uart (this also enables access to it registers)
    put32(uart_base_address + AUX_MU_CNTL_REG_OFFSET, 0);    // Disable auto flow control and disable receiver and transmitter (for now)
    put32(uart_base_address + AUX_MU_IER_REG_OFFSET, 1);     // Enable receive interrupts
    put32(uart_base_address + AUX_MU_LCR_REG_OFFSET, 3);     // Enable 8 bit mode
    put32(uart_base_address + AUX_MU_MCR_REG_OFFSET, 0);     // Set RTS line to be always high
    put32(uart_base_address + AUX_MU_BAUD_REG_OFFSET, 270);  // Set baud rate to 115200

    put32(uart_base_address + AUX_MU_CNTL_REG_OFFSET, 3);    // Finally, enable transmitter and receiver
}

// This function is required by printf function
void putc(void* p, char c)
{
    uart_send(c);
}
