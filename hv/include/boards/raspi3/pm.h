#ifndef _BOARDS_RASPI3_PM_H
#define _BOARDS_RASPI3_PM_H

#include "peripherals/base.h"

#define PM_PASSWORD 0x5a000000
#define PM_RSTC     (PBASE + 0x0010001c)
#define PM_WDOG     (PBASE + 0x00100024)

#endif /* _BOARDS_RASPI3_PM_H */
