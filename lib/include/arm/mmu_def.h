#ifndef _LIB_ARM_MMU_DEF_H
#define _LIB_ARM_MMU_DEF_H

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
//   Descriptor type[1:0] (Stage 1/2 共通)

// nG[11] not Global
//   0b0: このエントリはグローバルなのですべてのプロセスで使う(TLB から簡単には消えない)
//   0b1: このエントリは特定プロセス用(ASID に紐づけられ、ASID が切り替わると TLB から消えるかもしれない)
#define MM_S1_nG_GLOBAL             (0b0 << 11)
#define MM_S1_nG_NON_GLOBAL         (0b1 << 11)

// AF[10] Access flag
//   0b0: アクセス拒否、アクセスすると例外が発生する
//   0b1: アクセス許可
#define MM_S1_AF_NO_ACCESS          (0b0 << 10)
#define MM_S1_AF_ACCESS             (0b1 << 10)

// SH[9:8] Shareability field
// キャッシュの制御に使われる
//   0b00: Non-shareable
//     他コアからはアクセスされず占有する領域、キャッシュ一貫性を保つ処理が省かれ効率化される
//   0b10: Outer Shareable
//     他コアだけではなく GPU などの他 IP からもアクセスされる領域
//   0b11: Inner Shareable
//     他コアからアクセスされる領域、ハードウェアがキャッシュ一貫性を担保する
#define MM_S1_SH_NON_SHAREABLE      (0b00 << 8)
#define MM_S1_SH_OUTER_SHAREABLE    (0b10 << 8)
#define MM_S1_SH_INNER_SHAREABLE    (0b11 << 8)

// AP[7:6] Access Permissions
//   0b00: EL1 Read/Write, EL0 No Access
//   0b01: EL1 Read/Write, EL0 Read/Write
//   0b10: EL1 Read-Only, EL0 No Access
//   0b11: EL1 Read-Only, EL0 Read-Only
// ARMv7 だと EL を PL と呼ぶので注意
#define MM_S1_AP_RW_EL1             (0b00 << 6)
#define MM_S1_AP_RW_ALL             (0b01 << 6)
#define MM_S1_AP_RO_EL1             (0b10 << 6)
#define MM_S1_AP_RO_ALL             (0b11 << 6)

// NS[5] Non-seccure
//   0b0: secure
//   0b1: non-secure
#define MM_S1_NS_SECURE             (0b0 << 5)
#define MM_S1_NS_NONSECURE          (0b1 << 5)

// AttrIndx[4:2] Stage 1 memory attributes index
//   MAIR_EL1 レジスタのどのパートを使うかを指定する
//   https://developer.arm.com/documentation/ddi0601/2024-09/AArch64-Registers/MAIR-EL1--Memory-Attribute-Indirection-Register--EL1-
//   定数定義はなし

// Descriptor type[1:0]
//   レベル0,1,2(PGD, PUD, PMD) の変換の場合
//     0b11: アドレスが指すのは次のレベルのテーブル
//     0b01: アドレスが指すのはブロック
//           探索を途中でやめて、それ以下のレベルのページに相当する領域をまとめてマップすることになる
//   レベル3(PTE)　の変換の場合
//     0b11: アドレスが指すのはページ
//     0b01: invalid
//   仮にブロックを使わない場合、全エントリの下位2ビットはすべて 0x3 になる
#define MM_TYPE_PAGE_TABLE          (0b11 << 0)
#define MM_TYPE_PAGE                (0b11 << 0)
#define MM_TYPE_BLOCK               (0b01 << 0)


// Stage 2 のエントリの attribute の設定用定数
//   Attribute fields in stage 2 Long-descriptor Block and Page descriptors
//   https://developer.arm.com/documentation/ddi0406/c/System-Level-Architecture/Virtual-Memory-System-Architecture--VMSA-/Long-descriptor-translation-table-format/Memory-attributes-in-the-Long-descriptor-translation-table-format-descriptors?lang=en
// ビットアサインは以下
//   RES0[11]
//   AF[10]
//   SH[9:8]
//   HAP[7:6]
//   MemAttr[5:2]
//   Descriptor type[1:0] (Stage 1/2 共通)

// RES0 (Stage 2 では nG はない)
#define MM_S2_RES0                  (0b0 << 11)

// AF[10] Access flag (Stage 1 と同じ)
#define MM_S2_AF_NO_ACCESS          (0b0 << 10)
#define MM_S2_AF_ACCESS             (0b1 << 10)

// SH[9:8] Shareability field (Stage 1 と同じ)
// S1 と S2 の SH の設定を組み合わせて最終的なキャッシュの制御が決まる
#define MM_S2_SH_NON_SHAREABLE       (0b00 << 8)
#define MM_S2_SH_OUTER_SHAREABLE     (0b10 << 8)
#define MM_S2_SH_INNER_SHAREABLE     (0b11 << 8)

// HAP[7:6]: Stage 2 access permissions bits
//   0b00: アクセス禁止
//   0b01: 読み取りのみ許可され、Stage 1 のアクセス許可に関係なく書き込みは許可されない
//   0b10: 書き込みのみ許可され、Stage 1 のアクセス許可に関係なく読み取りは許可されない
//   0b11: 読み書きを許可、つまり Stage 1 のアクセス許可がそのまま適用される
#define MM_S2_HAP_NONE              (0b00 << 6)
#define MM_S2_HAP_RO                (0b01 << 6)
#define MM_S2_HAP_WO                (0b10 << 6)
#define MM_S2_HAP_RW                (0b11 << 6)

// MemAttr[5:2]: Stage 2 memory attributes
// まず MemAttr[3:2] で Cacheability が決まる
//   0b00: Strongly-ordered or device memory で Cacheability は適用外
//   0b01: Normal memory で Outer non-cacheable
//   0b10: Normal memory で Outer Write-Through cacheable
//   0b11: Normal memory で Outer Write-Back cacheable
// MemAttr[1:0] の意味は MemAttr[3:2] で変わる
//   MemAttr[3:2] が 0b00 のとき
//     0b00: Strongly-ordered memory
//     0b01: Device memory
//     0b10: unpredictable
//     0b11: unpredictable
//   MemAttr[3:2] が 0b00 以外のとき
//     0b00: unpredictable
//     0b01: Inner non-cacheable
//     0b10: Inner Write-Through cacheable
//     0b11: Inner Write-Back cacheable
// 現状 inner が write-through で outer が write-back といった特殊な組み合わせは定義していない
#define MM_S2_MEMATTR_STRONGLY_ORDERED   (0b0000 << 2)
#define MM_S2_MEMATTR_DEVICE             (0b0001 << 2)
#define MM_S2_MEMATTR_NORMAL_NC          (0b0101 << 2)
#define MM_S2_MEMATTR_NORMAL_WT          (0b1010 << 2)
#define MM_S2_MEMATTR_NORMAL_WB          (0b1111 << 2)

// Descriptor type[1:0] は Stage 1 と同じなので定義なし


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
