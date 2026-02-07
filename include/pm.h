#ifndef _PM_H
#define _PM_H

#include "peripherals/base.h"

#define PM_PASSWORD 0x5a000000
#define PM_RSTC     (PBASE + 0x0010001c)
#define PM_WDOG     (PBASE + 0x00100024)

void system_shutdown(void);

#endif
