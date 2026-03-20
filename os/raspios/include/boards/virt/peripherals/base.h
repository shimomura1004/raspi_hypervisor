#ifndef	_P_BASE_H
#define	_P_BASE_H

#include "mm.h"

#define PHYS_MEMORY_BASE    0x40000000
#define RAM_SIZE            0x40000000

// Virt GIC Base
#define DEVICE_BASE         0x08000000	
#define PBASE               (VA_START + DEVICE_BASE)

#endif  /*_P_BASE_H */
