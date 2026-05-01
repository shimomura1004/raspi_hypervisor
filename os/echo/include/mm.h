#ifndef _MM_H
#define _MM_H

#include "arm/mm.h"

#define LOW_MEMORY      (2 * SECTION_SIZE)

#ifndef __ASSEMBLER__

void memzero(unsigned long src, unsigned long n);

#endif

#endif  /*_MM_H */
