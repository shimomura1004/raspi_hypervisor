#ifndef _MMU_H
#define _MMU_H

#include "arm/mmu_def.h"

// MAIR_EL1
//   MAIR_EL1 レジスタは8個のパートに分かれており、ページの属性を入れておくことができる
//   システム全体でメモリの属性は8パターンしかないということ
//   各ページは変換テーブルのエントリで MAIR のどのパートを使うかを
//   ページテーブルのエントリの下位12ビット内の AttrIndex[4:2] で指定する
//
// raspvisor では以下の2パターンを準備している
//   0b000: DEVICE_nGnRnE       デバイスメモリ
//   0b001: NORMAL_CACHEABLE    通常のメモリで、キャッシュも可能
//
// Memory region attributes:
//   n = AttrIndx[2:0]
//                      n   MAIR
//   DEVICE_nGnRnE    = 000 00000000
//   NORMAL_CACHEABLE = 001 11111111

// AttrIndex に入れる MAIR のインデックスの定義
#define MT_IDX_DEVICE_nGnRnE        0x0
#define MT_IDX_NORMAL_CACHEABLE     0x1

// 各メモリタイプ(MT)の設定値
#define MT_ATTR_DEVICE_nGnRnE       0x00
#define MT_ATTR_NORMAL_NC           0x44
#define MT_ATTR_NORMAL_WB           0xFF

// MAIR_EL1 レジスタに設定する値
// デバッグのため、いったんキャッシュを無効(MT_ATTR_NORMAL_NC)にしている
#define MAIR_VALUE \
        ((MT_ATTR_DEVICE_nGnRnE << (8 * MT_IDX_DEVICE_nGnRnE)) | \
         (MT_ATTR_NORMAL_NC     << (8 * MT_IDX_NORMAL_CACHEABLE)))

// 以下2つのページ属性用のフラグは Stage 1 のエントリ用
#define MMU_FLAGS \
    (MM_TYPE_BLOCK | (MT_IDX_NORMAL_CACHEABLE << 2) | MM_S1_nG_NON_GLOBAL | MM_S1_AF_ACCESS)
#define MMU_DEVICE_FLAGS \
    (MM_TYPE_BLOCK | (MT_IDX_DEVICE_nGnRnE << 2) | MM_S1_nG_NON_GLOBAL | MM_S1_AF_ACCESS)

// todo: mmu_def.h で定義された値を使う

#define MM_STAGE2_ACCESS    (1 << 10)
#define MM_STAGE2_SH        (3 << 8)    // inner shareable
#define MM_STAGE2_AP        (3 << 6)    // read/write
// todo: いったんキャッシュを無効にする
//#define MM_STAGE2_MEMATTR   (0xf << 2)  // Write-back cacheable
#define MM_STAGE2_MEMATTR   (0x5 << 2)  // Write-back cacheable

// 通常のメモリのエントリ
#define MMU_STAGE2_PAGE_FLAGS \
    (MM_TYPE_PAGE | MM_STAGE2_ACCESS | MM_STAGE2_SH | \
     MM_STAGE2_AP | MM_STAGE2_MEMATTR)

#define MM_STAGE2_AP_NONE           (0 << 6)    // No access permitted
#define MM_STAGE2_DEVICE_MEMATTR    (0x0 << 2)  // Strongly-ordered memory

// デバイスの MMIO 用のエントリ
// MM_STAGE2_AP -> MM_STAGE2_AP_NONE に変更
//   (read/write) -> (no access)
// MM_STAGE2_MEMATTR -> MM_STAGE2_DEVICE_MEMATTR に変更
//   (inner write-back cacheable) -> (strongly-ordered)
#define MMU_STAGE2_MMIO_FLAGS \
    (MM_TYPE_PAGE | MM_STAGE2_ACCESS | MM_STAGE2_SH | \
     MM_STAGE2_AP_NONE | MM_STAGE2_DEVICE_MEMATTR)

#define TCR_T0SZ			(64 - 48)
#define TCR_TG0_4K			(0 << 14)
#define TCR_VALUE			(TCR_T0SZ | TCR_TG0_4K)

#endif
