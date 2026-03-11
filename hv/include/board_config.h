#ifndef _BOARD_CONFIG_H
#define _BOARD_CONFIG_H

#if defined(BOARD_RASPI3)
    #include "boards/raspi3.h"
#elif defined(BOARD_VIRT)
    #include "boards/virt.h"
#else
    #error "Unknown or undefined BOARD. Please define BOARD_RASPI3 or BOARD_VIRT."
#endif

#endif /* _BOARD_CONFIG_H */
