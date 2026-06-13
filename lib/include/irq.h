#ifndef _LIB_IRQ_H
#define _LIB_IRQ_H

#include "arm/irq.h"

#if defined(BOARD_RASPI3)
#include "qa7.h"
#include "lic.h"
#elif defined(BOARD_VIRT)
#include "gic.h"
#endif

#endif  /*_LIB_IRQ_H */
