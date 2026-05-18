#ifndef _BOARD_CONFIG_H
#define _BOARD_CONFIG_H

#if defined(BOARD_RASPI3)
    #include "boards/raspi3/raspi3.h"
    #include "peripherals/mini_uart_regs.h"
    #include "peripherals/gpio_regs.h"
    #include "bcm2836_armctrl_regs.h"
    #include "qa7_regs.h"
    #include "peripherals/mailbox_regs.h"
    #include "peripherals/systimer_regs.h"
#elif defined(BOARD_VIRT)
    #include "boards/virt/virt.h"
    #include "peripherals/pl011_uart_regs.h"
    #include "gic_regs.h"
#else
    #error "Unknown or undefined BOARD. Please define BOARD_RASPI3 or BOARD_VIRT."
#endif

#endif /* _BOARD_CONFIG_H */
