#ifndef	_P_PL011_UART_H
#define	_P_PL011_UART_H

#include "peripherals/base.h"

// qemu virt board のメモリマップは qemu のソースコードで定義されている
// https://github.com/qemu/qemu/blob/master/hw/arm/virt.c
// [VIRT_UART0] = { 0x09000000, 0x00001000 }
#define UART_BASE   (0x09000000)

#define UART_DR     (UART_BASE + 0x00)
#define UART_FR     (UART_BASE + 0x18)
#define UART_IBRD   (UART_BASE + 0x24)
#define UART_FBRD   (UART_BASE + 0x28)
#define UART_LCRH   (UART_BASE + 0x2c)
#define UART_CR     (UART_BASE + 0x30)
#define UART_IMSC   (UART_BASE + 0x38)
#define UART_ICR    (UART_BASE + 0x44)

// UARTDR
//   https://developer.arm.com/documentation/ddi0183/f/programmer-s-model/register-descriptions/data-register--uartdr
//   [15:12] Reserved
//   [11] OE
//     Overrun error. This bit is set to 1 if data is received and the receive FIFO is already full.
//     This is cleared to 0 once there is an empty space in the FIFO and a new character can be written to it.
//   [10] BE
//     Break error. This bit is set to 1 if a break condition was detected, indicating that the received data input was held LOW for longer than a full-word transmission time (defined as start, data, parity and stop bits).
//     In FIFO mode, this error is associated with the character at the top of the FIFO. When a break occurs, only one 0 character is loaded into the FIFO. The next character is only enabled after the receive data input goes to a 1 (marking state), and the next valid start bit is received.
//   [9] PE
//     Parity error. When this bit is set to 1, it indicates that the parity of the received data character does not match the parity selected as defined by bits 2 and 7 of the UARTLCR_H register.
//     In FIFO mode, this error is associated with the character at the top of the FIFO.
//   [8] FE
//     Framing error. When this bit is set to 1, it indicates that the received character did not have a valid stop bit (a valid stop bit is 1).
//     In FIFO mode, this error is associated with the character at the top of the FIFO.
//   [7:0] DATA
//     Receive (read) data character.
//     Transmit (write) data character.

// UARTFR
//   https://developer.arm.com/documentation/ddi0183/f/programmer-s-model/register-descriptions/flag-register--uartfr
//   [15:9] Reserved, do not modify, read as zero.
//   [8] RI: Ring indicator
//     This bit is the complement of the UART ring indicator (nUARTRI) modem status input. That is, the bit is 1 when the modem status input is 0.
//   [7] TXFE: Transmit FIFO empty
//     The meaning of this bit depends on the state of the FEN bit in the UARTLCR_H register.
//     If the FIFO is disabled, this bit is set when the transmit holding register is empty.
//     If the FIFO is enabled, the TXFE bit is set when the transmit FIFO is empty.
//     This bit does not indicate if there is data in the transmit shift register.
//   [6] RXFF: Receive FIFO full
//     The meaning of this bit depends on the state of the FEN bit in the UARTLCR_H register.
//     If the FIFO is disabled, this bit is set when the receive holding register is full.
//     If the FIFO is enabled, the RXFF bit is set when the receive FIFO is full.
//   [5] TXFF: Transmit FIFO full
//     The meaning of this bit depends on the state of the FEN bit in the UARTLCR_H register.
//     If the FIFO is disabled, this bit is set when the transmit holding register is full.
//     If the FIFO is enabled, the TXFF bit is set when the transmit FIFO is full.
//   [4] RXFE: Receive FIFO empty
//     The meaning of this bit depends on the state of the FEN bit in the UARTLCR_H register.
//     If the FIFO is disabled, this bit is set when the receive holding register is empty.
//     If the FIFO is enabled, the RXFE bit is set when the receive FIFO is empty.
//   [3] BUSY: UART busy.
//     If this bit is set to 1, the UART is busy transmitting data. This bit remains set until the complete byte, including all the stop bits, has been sent from the shift register.
//     This bit is set as soon as the transmit FIFO becomes non-empty (regardless of whether the UART is enabled or not).
//   [2] DCD: Data carrier detect
//     This bit is the complement of the UART data carrier detect (nUARTDCD) modem status input. That is, the bit is 1 when the modem status input is 0.
//   [1] DSR: Data set ready
//     This bit is the complement of the UART data set ready (nUARTDSR) modem status input. That is, the bit is 1 when the modem status input is 0.
//   [0] CTS: Clear to send
//     This bit is the complement of the UART clear to send (nUARTCTS) modem status input. That is, the bit is 1 when the modem status input is 0.
#define UART_FR_TXFF (1 << 5) /* Transmit FIFO full */
#define UART_FR_RXFE (1 << 4) /* Receive FIFO empty */

// UARTIMSC: Interrupt mask set/clear register
//   https://developer.arm.com/documentation/ddi0183/f/programmer-s-model/register-descriptions/interrupt-mask-set-clear-register--uartimsc
//   割込みマスクレジスタで、読むとマスク状態が返され、1 を書くとマスクされ、0 を書くとマスク解除される
//   [15:11] Reserved, read as zero, do not modify.
//   [10] OEIM: Overrun error interrupt mask
//   [9] BEIM: Break error interrupt mask
//   [8] PEIM: Parity error interrupt mask
//   [7] FEIM: Framing error interrupt mask
//   [6] RTIM: Receive timeout interrupt mask
//   [5] TXIM: Transmit interrupt mask
//   [4] RXIM: Receive interrupt mask
//   [3] DSRMIM: nUARTDSR modem interrupt mask
//   [2] DCDMIM: nUARTDCD modem interrupt mask
//   [1] CTSMIM: nUARTCTS modem interrupt mask
//   [0] RIMIM: nUARTRI modem interrupt mask
#define UART_IMSC_RXIM (1 << 4) /* Receive interrupt mask */

// UARTICR: Interrupt clear register
//   https://developer.arm.com/documentation/ddi0183/f/programmer-s-model/register-descriptions/interrupt-clear-register--uarticr
//   割込みクリアレジスタで、1 を書き込むと対応する割込みがクリアされる、0 を書き込んでも変化なし
//   [15:11] Reserved, read as zero, do not modify
//   [10] OEIC: Overrun error interrupt clear
//   [9] BEIC: Break error interrupt clear
//   [8] PEIC: Parity error interrupt clear
//   [7] FEIC: Framing error interrupt clear
//   [6] RTIC: Receive timeout interrupt clear
//   [5] TXIC: Transmit interrupt clear
//   [4] RXIC: Receive interrupt clear
//   [3] DSRMIC: nUARTDSR modem interrupt clear
//   [2] DCDMIC: nUARTDCD modem interrupt clear
//   [1] CTSMIC: nUARTCTS modem interrupt clear
//   [0] RIMIC: nUARTRI modem interrupt clear
#define UART_ICR_RXIC  (1 << 4) /* Receive interrupt clear */

void uart_init(void);
void handle_uart_irq(void);

#endif  /*_P_PL011_UART_H */
