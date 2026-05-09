#ifndef _P_TIMER_REGS_H
#define _P_TIMER_REGS_H

// システムタイマは4個あるが 0 と 2 は GPU で使われる

// System Timer Registers
// CS: System Timer Control/Status
//       Write a one to the relevant bit to clear the match
//       detect status bit and the corresponding interrupt request line. 
//   [31:4] unused
//   [3] M3: System Timer Match 3
//     0b0: No Timer 3 match since last cleared.
//     0b1: Timer 3 match detected
//   [2] M2: System Timer Match 2
//   [1] M1: System Timer Match 1
//   [0] M0: System Timer Match 0
// CLO: System Timer Counter Lower bits
//   [31:0] Lower 32-bits of the free running counter value
// CHI: System Timer Counter Higher bits
//   [31:0] Higher 32-bits of the free running counter value
// C0, C1, C2, C3: System Timer Compare
//   [31:0] Compare value for match channel n

#define TIMER_CS_OFFSET     (0x00)
#define TIMER_CLO_OFFSET    (0x04)
#define TIMER_CHI_OFFSET    (0x08)
#define TIMER_C0_OFFSET     (0x0C)
#define TIMER_C1_OFFSET     (0x10)
#define TIMER_C2_OFFSET     (0x14)
#define TIMER_C3_OFFSET     (0x18)

#define TIMER_BASE          (PBASE      + 0x00003000)
#define TIMER_CS            (TIMER_BASE + TIMER_CS_OFFSET)
#define TIMER_CLO           (TIMER_BASE + TIMER_CLO_OFFSET)
#define TIMER_CHI           (TIMER_BASE + TIMER_CHI_OFFSET)
#define TIMER_C0            (TIMER_BASE + TIMER_C0_OFFSET)
#define TIMER_C1            (TIMER_BASE + TIMER_C1_OFFSET)
#define TIMER_C2            (TIMER_BASE + TIMER_C2_OFFSET)
#define TIMER_C3            (TIMER_BASE + TIMER_C3_OFFSET)

#define TIMER_CS_M0         (1 << 0)
#define TIMER_CS_M1         (1 << 1)
#define TIMER_CS_M2         (1 << 2)
#define TIMER_CS_M3         (1 << 3)

#endif  /*_P_TIMER_REGS_H */
