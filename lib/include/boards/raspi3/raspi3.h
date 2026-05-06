#ifndef _BOARDS_RASPI3_H
#define _BOARDS_RASPI3_H

// Raspberry Pi 3B Specific Constants (物理アドレス)

// これは Arm コア側からみたアドレス(物理アドレス)
//   videocore からみたアドレスもある(バスアドレス)
// https://www.reddit.com/r/osdev/comments/uc98tz/raspberry_pi_3_base_peripheral_address/#:~:text=Physical%20addresses%20range%20from%200x3F000000,address%20range%20starting%20at%200x7E000000.

// BCM2837 ではペリフェラルが2つのブロックに分かれて配置されている
//   0x3F000000 - 0x3FFFFFFF : General Purpose I/O
//   0x40000000 - 0x401FFFFF : Local Peripherals

// todo: これらが物理アドレスであることを名前で明示する
#define RAM_BASE                0x00000000
#define RAM_SIZE                0x3F000000

// 利用可能な物理メモリの終端
// raspi3 では DRAM 末尾にデバイス領域(DEVICE_BASE~)が重なっているため、その手前までを RAM として扱う
#define HIGH_MEMORY             0x3F000000

// BCM2837 Peripheral Base
#define PERIPHERAL_BASE         0x3F000000
#define DEVICE_BASE             PERIPHERAL_BASE
#define DEVICE_SIZE             0x01000000

// ARM Local Peripheral Base (QA7)
#define LOCAL_PERIPHERAL_BASE   0x40000000
#define LOCAL_PERIPHERAL_SIZE   0x00200000

// Use Mini UART for Raspberry Pi 3
#define UART_TYPE_MINI          1

#endif /* _BOARDS_RASPI3_H */
