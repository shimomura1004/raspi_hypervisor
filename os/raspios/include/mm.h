#ifndef	_MM_H
#define	_MM_H

#include "peripherals/base.h"

#define VA_START 			0xffff000000000000

#define PAGE_MASK			0xfffffffffffff000
#define PAGE_SHIFT	 		12
#define TABLE_SHIFT 			9
#define SECTION_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE   			(1 << PAGE_SHIFT)	
#define SECTION_SIZE			(1 << SECTION_SHIFT)	

#define LOW_MEMORY              	(RAM_BASE + 2 * SECTION_SIZE)
#if defined(BOARD_VIRT)
#define HIGH_MEMORY             	(RAM_BASE + DEVICE_BASE)
#else
#define HIGH_MEMORY             	(RAM_BASE + RAM_SIZE)
#endif

#define PAGING_MEMORY 			(HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES 			(PAGING_MEMORY/PAGE_SIZE)

#define PTRS_PER_TABLE			(1 << TABLE_SHIFT)

#define PGD_SHIFT			PAGE_SHIFT + 3*TABLE_SHIFT
#define PUD_SHIFT			PAGE_SHIFT + 2*TABLE_SHIFT
#define PMD_SHIFT			PAGE_SHIFT + TABLE_SHIFT

#define PG_DIR_SIZE			(3 * PAGE_SIZE)

#define STACK_SIZE			0x400000

#ifndef __ASSEMBLER__

// 受けとった物理アドレス(pa)から、ボード上のメモリの開始位置(RAM_BASE)を引いてオフセットを計算
// それに仮想アドレスの開始位置(VA_START)を足すことで、仮想アドレスに変換
#define PHYS_TO_VIRT(pa) ((pa) - RAM_BASE + VA_START)
#define VIRT_TO_PHYS(va) ((va) - VA_START + RAM_BASE)

struct task_struct;

unsigned long get_free_page();
void free_page(unsigned long p);
void map_page(struct task_struct *task, unsigned long va, unsigned long page);

int copy_virt_memory(struct task_struct *dst); 
unsigned long allocate_kernel_page(); 
unsigned long allocate_user_page(struct task_struct *task, unsigned long va); 

extern unsigned long pg_dir;

#endif

#endif  /*_MM_H */
