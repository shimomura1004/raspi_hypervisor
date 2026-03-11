#ifndef _BOARDS_RASPI3_H
#define _BOARDS_RASPI3_H

// Raspberry Pi 3B Specific Constants

#define RAM_BASE                0x00000000
#define RAM_SIZE                0x40000000      // 1GB
#define VA_START                0x0000000000000000

// BCM2837 Peripheral Base
#define PERIPHERAL_BASE         0x3F000000

// ARM Local Peripheral Base (QA7)
#define LOCAL_PERIPHERAL_BASE   0x40000000

#define UART_TYPE_MINI          1

#endif /* _BOARDS_RASPI3_H */
