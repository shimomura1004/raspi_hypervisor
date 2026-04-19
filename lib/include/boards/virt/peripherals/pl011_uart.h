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

// UARTDR: Data register
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

// UARTFR: Flag register
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

// UARTLCR_H: Line control register
//   https://developer.arm.com/documentation/ddi0183/f/programmer-s-model/register-descriptions/line-control-register--uartlcr-h
//   [15:8] Reserved, do not modify, read as zero.
//   [7] SPS: Stick parity select
//     When bits 1, 2, and 7 of the UARTLCR_H register are set, the parity bit is transmitted and checked as a 0. When bits 1 and 7 are set, and bit 2 is 0, the parity bit is transmitted and checked as a 1. When this bit is cleared stick parity is disabled. Refer to Table 3.11 for a truth table showing the SPS, EPS and PEN bits.
//   [6:5] WLEN: Word length
//     The select bits indicate the number of data bits transmitted or received in a frame as follows:
//     11 = 8 bits
//     10 = 7 bits
//     01 = 6 bits
//     00 = 5 bits.
//   [4] FEN: Enable FIFOs
//      If this bit is set to 1, transmit and receive FIFO buffers are enabled (FIFO mode). When cleared to 0 the FIFOs are disabled (character mode) that is, the FIFOs become 1-byte-deep holding registers.
//   [3] STP2: Two stop bits select
//      If this bit is set to 1, two stop bits are transmitted at the end of the frame. The receive logic does not check for two stop bits being received.
//   [2] EPS: Even parity select
//     If this bit is set to 1, even parity generation and checking is performed during transmission and reception, which checks for an even number of 1s in data and parity bits. When cleared to 0 then odd parity is performed which checks for an odd number of 1s. This bit has no effect when parity is disabled by Parity Enable (bit 1) being cleared to 0. Refer to Table 3.11 for a truth table showing the SPS, EPS and PEN bits.
//   [1] PEN: Parity enable
//     If this bit is set to 1, parity checking and generation is enabled, else parity is disabled and no parity bit added to the data frame. Refer to Table 3.11 for a truth table showing the SPS, EPS and PEN bits.
//   [0] BRK: Send break
//     If this bit is set to 1, a low-level is continually output on the UARTTXD output, after completing transmission of the current character. For the proper execution of the break command, the software must set this bit for at least two complete frames.
//     For normal use, this bit must be cleared to 0.
#define UART_LCRH_WLEN_8BIT (3 << 5)
#define UART_LCRH_FEN       (1 << 4)

// UARTCR: Control register
//   https://developer.arm.com/documentation/ddi0183/f/programmer-s-model/register-descriptions/control-register--uartcr
//    [15] CTSEn: CTS hardware flow control enable
//      If this bit is set to 1, CTS hardware flow control is enabled. Data is only transmitted when the nUARTCTS signal is asserted.
//    [14] RTSEn: RTS hardware flow control enable
//      If this bit is set to 1, RTS hardware flow control is enabled. Data is only requested when there is space in the receive FIFO for it to be received.
//    [13] Out2
//    This bit is the complement of the UART Out2 (nUARTOut2) modem status output. That is, when the bit is programmed to a 1, the output is 0. For DTE this can be used as Ring Indicator (RI).
//    [12] Out1
//    This bit is the complement of the UART Out1 (nUARTOut1) modem status output. That is, when the bit is programmed to a 1 the output is 0. For DTE this can be used as Data Carrier Detect (DCD).
//    [11] RTS: Request to send
//      This bit is the complement of the UART request to send (nUARTRTS) modem status output. That is, when the bit is programmed to a 1, the output is 0.
//    [10] DTR: Data transmit ready
//      This bit is the complement of the UART data transmit ready (nUARTDTR) modem status output. That is, when the bit is programmed to a 1, the output is 0.
//    [9] RXE: Receive enable
//      If this bit is set to 1, the receive section of the UART is enabled. Data reception occurs for either UART signals or SIR signals according to the setting of SIR Enable (bit 1). When the UART is disabled in the middle of reception, it completes the current character before stopping.
//    [8] TXE: Transmit enable
//      If this bit is set to 1, the transmit section of the UART is enabled. Data transmission occurs for either UART signals, or SIR signals according to the setting of SIR Enable (bit 1). When the UART is disabled in the middle of transmission, it completes the current character before stopping.
//    [7] LBE: Loop back enable
//      If this bit is set to 1 and the SIR Enable bit is set to 1 and the test register UARTTCR bit 2 (SIRTEST) is set to 1, then the nSIROUT path is inverted, and fed through to the SIRIN path. The SIRTEST bit in the test register must be set to 1 to override the normal half-duplex SIR operation. This must be the requirement for accessing the test registers during normal operation, and SIRTEST must be cleared to 0 when loopback testing is finished.This feature reduces the amount of external coupling required during system test.
//      If this bit is set to 1, and the SIRTEST bit is set to 0, the UARTTXD path is fed through to the UARTRXD path.
//      In either SIR mode or normal mode, when this bit is set, the modem outputs are also fed through to the modem inputs.
//      This bit is cleared to 0 on reset, which disables the loopback mode.
//    [6:3] Reserved, do not modify, read as zero.
//    [2] SIRLP: IrDA SIR low power mode
//      This bit selects the IrDA encoding mode. If this bit is cleared to 0, low-level bits are transmitted as an active high pulse with a width of 3/16th of the bit period. If this bit is set to 1, low-level bits are transmitted with a pulse width which is 3 times the period of the IrLPBaud16 input signal, regardless of the selected bit rate. Setting this bit uses less power, but might reduce transmission distances.
//    [1] SIREN: SIR enable
//      If this bit is set to 1, the IrDA SIR ENDEC is enabled. This bit has no effect if the UART is not enabled by bit 0 being set to 1.
//      When the IrDA SIR ENDEC is enabled, data is transmitted and received on nSIROUT and SIRIN. UARTTXD remains in the marking state (set to 1). Signal transitions on UARTRXD or modem status inputs have no effect.
//      When the IrDA SIR ENDEC is disabled, nSIROUT remains cleared to 0 (no light pulse generated), and signal transitions on SIRIN have no effect.
//    [0] UARTEN: UART enable
//      If this bit is set to 1, the UART is enabled. Data transmission and reception occurs for either UART signals or SIR signals according to the setting of SIR Enable (bit 1). When the UART is disabled in the middle of transmission or reception, it completes the current character before stopping.
#define UART_CR_UARTEN (1 << 0)
#define UART_CR_TXE    (1 << 8)
#define UART_CR_RXE    (1 << 9)

// UARTIMSC: Interrupt mask set/clear register
//   https://developer.arm.com/documentation/ddi0183/f/programmer-s-model/register-descriptions/interrupt-mask-set-clear-register--uartimsc
//   割込みマスクレジスタ。1 を書くとその割込みが許可（有効）され、0 を書くと禁止（無効）される。
//   一般的には "マスクする" は "割込みを CPU に伝えないようにする" という意味で使われるが、Arm PrimeCell 周辺機器では逆になる
//     "CPU への割込み" = "デバイスからの割込み" and "マスク"
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
#define UART_IMSC_RTIM (1 << 6) /* Receive timeout interrupt mask */

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
#define UART_ICR_RTIC  (1 << 6) /* Receive timeout interrupt clear */

void uart_init(void);
void handle_uart_irq(void);

#endif  /*_P_PL011_UART_H */
