#ifndef _LIB_ARM_MM_H
#define _LIB_ARM_MM_H

#define PAGE_MASK           0xfffffffffffff000
#define PAGE_SHIFT          12
#define TABLE_SHIFT         9
#define SECTION_SHIFT       (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE           (1 << PAGE_SHIFT)
#define SECTION_SIZE        (1 << SECTION_SHIFT)

// 1テーブルあたりのエントリ数
#define PTRS_PER_TABLE      (1 << TABLE_SHIFT)

// ステージ 1 アドレス変換のための各インデックスの位置 (VA to IPA)
#define PGD_SHIFT           (PAGE_SHIFT + 3 * TABLE_SHIFT)
#define PUD_SHIFT           (PAGE_SHIFT + 2 * TABLE_SHIFT)
#define PMD_SHIFT           (PAGE_SHIFT +     TABLE_SHIFT)

#endif /* _LIB_ARM_MM_H */
