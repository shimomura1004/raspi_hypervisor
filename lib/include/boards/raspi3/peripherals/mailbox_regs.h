#ifndef _P_MAILBOX_REGS_H
#define _P_MAILBOX_REGS_H

#include "boards/raspi3/raspi3.h"

// Local Peripherals
// (Defined in board_config.h / boards/raspi3.h)

// Mailbox の割込みを有効化するためのレジスタ
#define IPI_CORE0_CONTROL_OFFSET    (0x50)
#define IPI_CORE1_CONTROL_OFFSET    (0x54)
#define IPI_CORE2_CONTROL_OFFSET    (0x58)
#define IPI_CORE3_CONTROL_OFFSET    (0x5C)

#define IPI_CORE0_CONTROL           (LOCAL_PERIPHERAL_BASE + IPI_CORE0_CONTROL_OFFSET)
#define IPI_CORE1_CONTROL           (LOCAL_PERIPHERAL_BASE + IPI_CORE1_CONTROL_OFFSET)
#define IPI_CORE2_CONTROL           (LOCAL_PERIPHERAL_BASE + IPI_CORE2_CONTROL_OFFSET)
#define IPI_CORE3_CONTROL           (LOCAL_PERIPHERAL_BASE + IPI_CORE3_CONTROL_OFFSET)

// MBOX_CORE[n]_CONTROL にセットするビット
//   FIQ    IRQ Destination
//   0      0   None (disabled)
//   0      1   IRQ
//   1      0   FIQ
//   1      1   FIQ
// FIQ を有効にすると、IRQ は無効になる
// MBOX_CONTROL_IRQ_[n]_BIT: Core[n] からの Mailbox の割込みを有効にする
#define IPI_CONTROL_FIQ_3_BIT   (1 << 7)
#define IPI_CONTROL_FIQ_2_BIT   (1 << 6)
#define IPI_CONTROL_FIQ_1_BIT   (1 << 5)
#define IPI_CONTROL_FIQ_0_BIT   (1 << 4)
#define IPI_CONTROL_IRQ_3_BIT   (1 << 3)
#define IPI_CONTROL_IRQ_2_BIT   (1 << 2)
#define IPI_CONTROL_IRQ_1_BIT   (1 << 1)
#define IPI_CONTROL_IRQ_0_BIT   (1 << 0)

// 割込みの発生要因を保持するレジスタ
//   Bits   Description
//   31-28  <Reserved>
//   17:12  Peripheral 1..15 interrupt (Currently not used)
//   11     Local timer interrupt
//   10     AXI-outstanding interrupt <For core 0 only!> all others are 0
//   9      PMU interrupt
//   8      GPU interrupt <Can be high in one core only>
//   7      Mailbox 3 interrupt
//   6      Mailbox 2 interrupt
//   5      Mailbox 1 interrupt
//   4      Mailbox 0 interrupt
//   3      CNTVIRQ interrupt
//   2      CNTHPIRQ interrupt
//   1      CNTPNSIRQ interrupt
//   0      CNTPSIRQ interrupt (Physical Timer -1)
#define CORE0_IRQ_SOURCE        (LOCAL_PERIPHERAL_BASE + 0x60)
#define CORE1_IRQ_SOURCE        (LOCAL_PERIPHERAL_BASE + 0x64)
#define CORE2_IRQ_SOURCE        (LOCAL_PERIPHERAL_BASE + 0x68)
#define CORE3_IRQ_SOURCE        (LOCAL_PERIPHERAL_BASE + 0x6C)

#define CORE0_FIQ_SOURCE        (LOCAL_PERIPHERAL_BASE + 0x70)
#define CORE1_FIQ_SOURCE        (LOCAL_PERIPHERAL_BASE + 0x74)
#define CORE2_FIQ_SOURCE        (LOCAL_PERIPHERAL_BASE + 0x78)
#define CORE3_FIQ_SOURCE        (LOCAL_PERIPHERAL_BASE + 0x7C)

#define IRQ_SOURCE_MBOX_0_BIT   (1 << 4)
#define IRQ_SOURCE_MBOX_1_BIT   (1 << 5)
#define IRQ_SOURCE_MBOX_2_BIT   (1 << 6)
#define IRQ_SOURCE_MBOX_3_BIT   (1 << 7)

// Mailbox Set registers for sending to other cores
#define IPI_CORE0_SET_BASE      (LOCAL_PERIPHERAL_BASE + 0x80)
#define IPI_CORE1_SET_BASE      (LOCAL_PERIPHERAL_BASE + 0x90)
#define IPI_CORE2_SET_BASE      (LOCAL_PERIPHERAL_BASE + 0xa0)
#define IPI_CORE3_SET_BASE      (LOCAL_PERIPHERAL_BASE + 0xb0)

#define IPI_CORE0_SET_0         (IPI_CORE0_SET_BASE + 0x0)     // Core0 -> Core0
#define IPI_CORE0_SET_1         (IPI_CORE0_SET_BASE + 0x4)     // Core0 -> Core1
#define IPI_CORE0_SET_2         (IPI_CORE0_SET_BASE + 0x8)     // Core0 -> Core2
#define IPI_CORE0_SET_3         (IPI_CORE0_SET_BASE + 0xC)     // Core0 -> Core3

#define IPI_CORE1_SET_0         (IPI_CORE1_SET_BASE + 0x0)     // Core1 -> Core0
#define IPI_CORE1_SET_1         (IPI_CORE1_SET_BASE + 0x4)     // Core1 -> Core1
#define IPI_CORE1_SET_2         (IPI_CORE1_SET_BASE + 0x8)     // Core1 -> Core2
#define IPI_CORE1_SET_3         (IPI_CORE1_SET_BASE + 0xC)     // Core1 -> Core3

#define IPI_CORE2_SET_0         (IPI_CORE2_SET_BASE + 0x0)     // Core2 -> Core0
#define IPI_CORE2_SET_1         (IPI_CORE2_SET_BASE + 0x4)     // Core2 -> Core1
#define IPI_CORE2_SET_2         (IPI_CORE2_SET_BASE + 0x8)     // Core2 -> Core2
#define IPI_CORE2_SET_3         (IPI_CORE2_SET_BASE + 0xC)     // Core2 -> Core3

#define IPI_CORE3_SET_0         (IPI_CORE3_SET_BASE + 0x0)     // Core3 -> Core0
#define IPI_CORE3_SET_1         (IPI_CORE3_SET_BASE + 0x4)     // Core3 -> Core1
#define IPI_CORE3_SET_2         (IPI_CORE3_SET_BASE + 0x8)     // Core3 -> Core2
#define IPI_CORE3_SET_3         (IPI_CORE3_SET_BASE + 0xC)     // Core3 -> Core3

// Mailbox Read/Clear registers for receiving from other cores
#define IPI_CORE0_RD_CLR_BASE   (LOCAL_PERIPHERAL_BASE + 0xC0)
#define IPI_CORE1_RD_CLR_BASE   (LOCAL_PERIPHERAL_BASE + 0xD0)
#define IPI_CORE2_RD_CLR_BASE   (LOCAL_PERIPHERAL_BASE + 0xE0)
#define IPI_CORE3_RD_CLR_BASE   (LOCAL_PERIPHERAL_BASE + 0xF0)

#define IPI_CORE0_RD_CLR_0      (IPI_CORE0_RD_CLR_BASE + 0x0)  // Core0 -> Core0
#define IPI_CORE0_RD_CLR_1      (IPI_CORE0_RD_CLR_BASE + 0x4)  // Core0 -> Core1
#define IPI_CORE0_RD_CLR_2      (IPI_CORE0_RD_CLR_BASE + 0x8)  // Core0 -> Core2
#define IPI_CORE0_RD_CLR_3      (IPI_CORE0_RD_CLR_BASE + 0xC)  // Core0 -> Core3

#define IPI_CORE1_RD_CLR_0      (IPI_CORE1_RD_CLR_BASE + 0x0)  // Core1 -> Core0
#define IPI_CORE1_RD_CLR_1      (IPI_CORE1_RD_CLR_BASE + 0x4)  // Core1 -> Core1
#define IPI_CORE1_RD_CLR_2      (IPI_CORE1_RD_CLR_BASE + 0x8)  // Core1 -> Core2
#define IPI_CORE1_RD_CLR_3      (IPI_CORE1_RD_CLR_BASE + 0xC)  // Core1 -> Core3

#define IPI_CORE2_RD_CLR_0      (IPI_CORE2_RD_CLR_BASE + 0x0)  // Core2 -> Core0
#define IPI_CORE2_RD_CLR_1      (IPI_CORE2_RD_CLR_BASE + 0x4)  // Core2 -> Core1
#define IPI_CORE2_RD_CLR_2      (IPI_CORE2_RD_CLR_BASE + 0x8)  // Core2 -> Core2
#define IPI_CORE2_RD_CLR_3      (IPI_CORE2_RD_CLR_BASE + 0xC)  // Core2 -> Core3

#define IPI_CORE3_RD_CLR_0      (IPI_CORE3_RD_CLR_BASE + 0x0)  // Core3 -> Core0
#define IPI_CORE3_RD_CLR_1      (IPI_CORE3_RD_CLR_BASE + 0x4)  // Core3 -> Core1
#define IPI_CORE3_RD_CLR_2      (IPI_CORE3_RD_CLR_BASE + 0x8)  // Core3 -> Core2
#define IPI_CORE3_RD_CLR_3      (IPI_CORE3_RD_CLR_BASE + 0xC)  // Core3 -> Core3

#endif
