#ifndef _MM_H
#define _MM_H

#include "board_config.h"

// ソフトウェア実装依存のメモリレイアウト定義
#define PAGE_SHIFT          12
#define TABLE_SHIFT         9
#define SECTION_SHIFT       (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE           (1 << PAGE_SHIFT)
#define SECTION_SIZE        (1 << SECTION_SHIFT)
#define PAGE_MASK           (~(PAGE_SIZE-1))

#define PTRS_PER_TABLE      (1 << TABLE_SHIFT)

#define PGD_SHIFT           (PAGE_SHIFT + 3 * TABLE_SHIFT)
#define PUD_SHIFT           (PAGE_SHIFT + 2 * TABLE_SHIFT)
#define PMD_SHIFT           (PAGE_SHIFT +     TABLE_SHIFT)

#define PBASE               (DEVICE_BASE)

#define LOW_MEMORY          (2 * SECTION_SIZE)

#endif  /*_MM_H */
