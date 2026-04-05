#ifndef _BOARDS_RASPI3_H
#define _BOARDS_RASPI3_H

// Raspberry Pi 3B Specific Constants (物理アドレス)

// これは Arm コア側からみたアドレス(物理アドレス)
//   videocore からみたアドレスもある(バスアドレス)
// https://www.reddit.com/r/osdev/comments/uc98tz/raspberry_pi_3_base_peripheral_address/#:~:text=Physical%20addresses%20range%20from%200x3F000000,address%20range%20starting%20at%200x7E000000.

#define RAM_BASE                0x00000000UL
#define RAM_SIZE                0x40000000UL
#define VA_START                0x0000000000000000UL

// BCM2837 Peripheral Base
#define PERIPHERAL_BASE         0x3F000000
#define DEVICE_BASE             PERIPHERAL_BASE

// ARM Local Peripheral Base (QA7)
#define LOCAL_PERIPHERAL_BASE   0x40000000

#define UART_TYPE_MINI          1

// VMM(ELF)の初期スタックポインタ
#define VMM_DEFAULT_STACK_ADDR  (RAM_BASE + 0x200000)

#endif /* _BOARDS_RASPI3_H */
