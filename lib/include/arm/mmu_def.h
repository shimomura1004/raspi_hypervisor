#ifndef _LIB_ARM_MMU_DEF_H
#define _LIB_ARM_MMU_DEF_H

// todo: ここでは arm 仕様に基づく定数だけ定義するようにする
//       たとえば nG[11] など、設定できる値の片方しか定義がないのは問題
//       ここでは全ての値を定義しておいて、hv/os などでそれを選んで使うようにする
// todo: el1, el2 でファイルを分ける？

// ARMv8-A MMU Descriptor Definitions (Long Descriptor Format)
// Stage 1 のエントリの attribute の設定用定数
//   Attribute fields in stage 1 Long-descriptor Block and Page descriptors
//   https://developer.arm.com/documentation/ddi0406/c/System-Level-Architecture/Virtual-Memory-System-Architecture--VMSA-/Long-descriptor-translation-table-format/Memory-attributes-in-the-Long-descriptor-translation-table-format-descriptors?lang=en
//   ページエントリの下位12ビットにそのページの属性を設定する
// ビットアサインは以下
//   ng[11]
//   AF[10]
//   SH[9:8]
//   AP[7:6]
//   NS[5]
//   AttrIndx[4:2]
//   Descriptor type[1:0]

// nG[11] not Global
//   0b0: このエントリはグローバルなのですべてのプロセスで使う(TLB から簡単には消えない)
//   0b1: このエントリは特定プロセス用(ASID に紐づけられ、ASID が切り替わると TLB から消えるかもしれない)
#define MM_nG                   (0b1 << 11)

// AF[10] Access flag
//   0b0: アクセス拒否、アクセスすると例外が発生する
//   0b1: アクセス許可
#define MM_ACCESS               (0b1 << 10)

// SH[9:8] Shareability field
// キャッシュの制御に使われる
//   0b00: Non-shareable
//     他コアからはアクセスされず占有する領域、キャッシュ一貫性を保つ処理が省かれ効率化される
//   0b10: Outer Shareable
//     他コアだけではなく GPU などの他 IP からもアクセスされる領域
//   0b11: Inner Shareable
//     他コアからアクセスされる領域、ハードウェアがキャッシュ一貫性を担保する
#define MM_SH_NON_SHAREABLE     (0b00 << 8)
#define MM_SH_OUTER_SHAREABLE   (0b10 << 8)
#define MM_SH_INNER_SHAREABLE   (0b11 << 8)

// AP[7:6] Access Permissions
//   0b00: EL1 Read/Write, EL0 No Access
//   0b01: EL1 Read/Write, EL0 Read/Write
//   0b10: EL1 Read-Only, EL0 No Access
//   0b11: EL1 Read-Only, EL0 Read-Only
// ARMv7 だと EL を PL と呼ぶので注意
#define MM_AP_RW_EL1            (0b00 << 6)
#define MM_AP_RW_ALL            (0b01 << 6)
#define MM_AP_RO_EL1            (0b10 << 6)
#define MM_AP_RO_ALL            (0b11 << 6)

// NS[5] Non-seccure
//   0b0: secure
//   0b1: non-secure
#define MM_NS                   (0b1 << 5)

// AttrIndx[4:2] Stage 1 memory attributes index
//   MAIR_EL1 レジスタのどのパートを使うかを指定する
//   https://developer.arm.com/documentation/ddi0601/2024-09/AArch64-Registers/MAIR-EL1--Memory-Attribute-Indirection-Register--EL1-
//

// Descriptor type[1:0]
//   レベル0,1,2(PGD, PUD, PMD) の変換の場合
//     0b11: アドレスが指すのは次のレベルのテーブル
//     0b01: アドレスが指すのはブロック
//           探索を途中でやめて、それ以下のレベルのページに相当する領域をまとめてマップすることになる
//   レベル3(PTE)　の変換の場合
//     0b11: アドレスが指すのはページ
//     0b01: invalid
//   仮にブロックを使わない場合、全エントリの下位2ビットはすべて 0x3 になる
#define MM_TYPE_PAGE_TABLE      (0b11 << 0)
#define MM_TYPE_PAGE            (0b11 << 0)
#define MM_TYPE_BLOCK           (0b01 << 0)
#define MM_TYPE_INVALID         (0b01 << 0)




// Stage 2 のエントリの attribute の説明
//   Attribute fields in stage 2 Long-descriptor Block and Page descriptors
//   https://developer.arm.com/documentation/ddi0406/c/System-Level-Architecture/Virtual-Memory-System-Architecture--VMSA-/Long-descriptor-translation-table-format/Memory-attributes-in-the-Long-descriptor-translation-table-format-descriptors?lang=en
//   HAP と MemAttr は Stage 1 とは意味が異なる
//
// HAP[7:6]: Stage 2 access permissions bits
//   0b00: No access permitted
//   0b01: Read-only. Writes to the region are not permitted, regardless of the stage 1 permissons.
//   0b10: Write-only. Reads from the region are not permitted, regardless of the stage 1 permissions.
//   0b11: Read/write. The stage 1 permissons determine the access permissions for the region.
// MemAttr[5:2]: Stage 2 memory attributes
//   0b00xx: Strongly-ordered or Device, determined by MemAtr[1:0]
//     0b0000: Region is Strongly-ordered memory
//     0b0001: Region is Device memory
//     0b0010: unpredictable
//     0b0011: unpredictable
//   0b--xx: Normal
//     0b--00: unpredictable
//     0b--01: Inner Non-cacheable
//     0b--10: Inner Write-Through Cacheable
//     0b--11: Inner Write-Back Cacheable

// HAP[7:6]: Stage 2 access permissions bits
#define MM_S2_HAP_NONE          (0x0 << 6)
#define MM_S2_HAP_RO            (0x1 << 6)
#define MM_S2_HAP_WO            (0x2 << 6)
#define MM_S2_HAP_RW            (0x3 << 6)


// todo: TCR にも EL1/EL2 がある
// // TCR Definitions
// #define TCR_T0SZ(x)             ((x) & 0x3f)
// #define TCR_T1SZ(x)             (((x) & 0x3f) << 16)
// #define TCR_TG0_4K              (0 << 14)
// #define TCR_TG0_64K             (1 << 14)
// #define TCR_TG0_16K             (2 << 14)
// #define TCR_TG1_16K             (1 << 30)
// #define TCR_TG1_4K              (2 << 30)
// #define TCR_TG1_64K             (3 << 30)

#endif /* _LIB_ARM_MMU_DEF_H */
