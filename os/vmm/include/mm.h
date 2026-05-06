#ifndef _MM_H
#define _MM_H

#define VA_START 0

#define LOW_MEMORY              (2 * SECTION_SIZE)

#define PHYS_TO_VIRT(pa) ((pa) - RAM_BASE + VA_START)
#define VIRT_TO_PHYS(va) ((va) - VA_START + RAM_BASE)

#endif  /*_MM_H */
