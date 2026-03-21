#ifndef	_P_BASE_H
#define	_P_BASE_H

#include "mm.h"

// BCM2837 ではペリフェラルが2つのブロックに分かれて配置されている
//   0x3F000000 - 0x3FFFFFFF : General Purpose I/O
//   0x40000000 - 0x401FFFFF : Local Peripherals

#define RAM_BASE                0x00000000
#define RAM_SIZE                0x3F000000
#define DEVICE_BASE             0x3F000000	
#define DEVICE_SIZE             0x01000000

// BCM2837 のときのみ必要になる Local Peripherals 用の定数定義
#define LOCAL_PERIPHERAL_BASE   0x40000000
#define LOCAL_PERIPHERAL_SIZE   0x00200000

// Peripheral base address
#define PBASE                   (VA_START + DEVICE_BASE)

#endif  /*_P_BASE_H */
