#ifndef	_P_BASE_H
#define	_P_BASE_H

#include "mm.h"

#define RAM_BASE            0x40000000
#define RAM_SIZE            0x40000000

// virt ボードではペリフェラルはひとつのブロックに配置されている
#define DEVICE_BASE         0x08000000	
#define DEVICE_SIZE         0x02000000

// Peripheral base address
#define PBASE               (VA_START + DEVICE_BASE)

#endif  /*_P_BASE_H */
