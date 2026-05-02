#ifndef _MMU_H
#define _MMU_H

#include "arm/mmu_def.h"

// Memory region attributes:
// HV と同じ設定になっているので詳細は hv/include/arm/mmu.h を参照
//   n = AttrIndx[2:0]
//                    n MAIR
//   DEVICE_nGnRnE  000 00000000
//   NORMAL_NC      001 01000100
#define MT_DEVICE_nGnRnE        0x0
#define MT_NORMAL_NC            0x1
#define MT_DEVICE_nGnRnE_FLAGS  0x00
#define MT_NORMAL_NC_FLAGS      0x44
#define MAIR_VALUE \
    ((MT_DEVICE_nGnRnE_FLAGS << (8 * MT_DEVICE_nGnRnE)) | \
     (MT_NORMAL_NC_FLAGS << (8 * MT_NORMAL_NC)))

#define MMU_FLAGS \
    (MM_TYPE_BLOCK | (MT_NORMAL_NC << 2) | MM_S1_AF_ACCESS)
#define MMU_DEVICE_FLAGS \
    (MM_TYPE_BLOCK | (MT_DEVICE_nGnRnE << 2) | MM_S1_AF_ACCESS)
#define MMU_PTE_FLAGS \
    (MM_TYPE_PAGE | (MT_NORMAL_NC << 2) | MM_S1_AF_ACCESS | MM_S1_AP_RW_ALL)

// TCR_EL1 に設定する値(上位・下位アドレス空間とも、48 ビットアドレスかつページサイズは 4KB)
#define TCR_VALUE \
    (TCR_T0SZ(64 - 48) | TCR_T1SZ(64 - 48) | TCR_TG0_4K | TCR_TG1_4K)

#endif
