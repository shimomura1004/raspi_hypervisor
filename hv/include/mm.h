#ifndef _MM_H
#define _MM_H

#include "board_config.h"

// ソフトウェア実装依存のメモリレイアウト定義
//   他のサブプロジェクトとはたまたま共通なだけ
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

#define VA_START            0x0000000000000000
#define PBASE               (VA_START + PA_DEVICE_BASE)

// todo: 現状使われてない
#define VA_RAM_START        0x40000000
// todo: RAM のサイズはボードごとに違うので、ボードごとに変わる
#define VA_RAM_END          (VA_RAM_START + 0x40000000 - 1)
#define VA_DEV_START        0x00000000
#define VA_DEV_END          (0x40000000 - 1)

// todo: PHYS_MEMORY_SIZE はボードごとの定義にするべきでは？あとこれは DRAM ではなくメモリ全体の大きさでは？
// DRAM のサイズ
// ボードごとに RAM_SIZE が定義されている
#define PHYS_MEMORY_SIZE    RAM_SIZE

// 利用可能な物理メモリの開始アドレス(物理)
// ハイパーバイザ自身がロードされる領域と各コアのスタックを避けるため PA_RAM_BASE から8セクション(16MB)分ずらす
#define LOW_MEMORY          (PA_RAM_BASE + 8 * SECTION_SIZE)

// 利用可能な物理メモリの終端(物理)
// 各ボードのヘッダ (raspi3.h, virt.h) で定義される HIGH_MEMORY を使用する
#ifndef HIGH_MEMORY
#error "HIGH_MEMORY must be defined in board header"
#endif

// low_memory~high_memory が自由に使えるメモリ
// ページングの対象になるメモリのサイズ
#define PAGING_MEMORY       (HIGH_MEMORY - LOW_MEMORY)
// 含まれるページの数
#define PAGING_PAGES        (PAGING_MEMORY / PAGE_SIZE)


// ステージ 2 アドレス変換のための各インデックスの位置 (IPA to PA)
//   https://developer.arm.com/documentation/102142/0100/Stage-2-translation
// for armv7
//   https://developer.arm.com/documentation/den0013/d/The-Memory-Management-Unit/Level-2-translation-tables
#define LV1_SHIFT           (PAGE_SHIFT + 2 * TABLE_SHIFT)
#define LV2_SHIFT           (PAGE_SHIFT +     TABLE_SHIFT)

#define PG_DIR_SIZE         (4 * PAGE_SIZE)

#ifndef __ASSEMBLER__

#include "sched.h"

void mm_init();

unsigned long get_free_page();
void free_page(void *p);
void map_stage2_page(struct vm_struct2 *vm, unsigned long ipa,
                     unsigned long page, unsigned long flags);
unsigned long allocate_page();
unsigned long allocate_vm_page(struct vm_struct2 *vm, unsigned long ipa);
void set_vm_page_notaccessable(struct vm_struct2 *vm, unsigned long va);
void free_vm_memory(struct mm_struct *mm);

int handle_mem_abort(unsigned long addr, unsigned long esr);

unsigned long get_ipa(unsigned long va);
unsigned long get_pa_2nd(unsigned long va);
extern unsigned long pg_dir;

#endif

#endif  /*_MM_H */
