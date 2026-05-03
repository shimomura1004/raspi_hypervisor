#ifndef _LIB_ARM_MMU_DEF_H
#define _LIB_ARM_MMU_DEF_H

// C とアセンブリの両方でリテラルを扱えるようにするマクロ
//   C コンパイラは数字リテラルを符号付き整数として解釈するので、最上位ビットをセットすると負の数として扱われる
//   これを避けるためにリテラルの末尾に UL をつけて符号なし整数リテラルとして扱わせるのが安全
//   ただしアセンブラは UL を理解できずエラーになるので、アセンブラでも使うヘッダでは _UL マクロでリテラルを定義する
#ifdef __ASSEMBLER__
#define _UL(x) x
#else
#define _UL(x) x##UL
#endif

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
#define MM_S1_nG_GLOBAL             (_UL(0x0) << 11)
#define MM_S1_nG_NON_GLOBAL         (_UL(0x1) << 11)

// AF[10] Access flag
//   0b0: アクセス拒否、アクセスすると例外が発生する
//   0b1: アクセス許可
#define MM_S1_AF_NO_ACCESS          (_UL(0x0) << 10)
#define MM_S1_AF_ACCESS             (_UL(0x1) << 10)

// SH[9:8] Shareability field
// キャッシュの制御に使われる
//   0b00: Non-shareable
//     他コアからはアクセスされず占有する領域、キャッシュ一貫性を保つ処理が省かれ効率化される
//   0b10: Outer Shareable
//     他コアだけではなく GPU などの他 IP からもアクセスされる領域
//   0b11: Inner Shareable
//     他コアからアクセスされる領域、ハードウェアがキャッシュ一貫性を担保する
#define MM_S1_SH_NON_SHAREABLE      (_UL(0x0) << 8)
#define MM_S1_SH_OUTER_SHAREABLE    (_UL(0x2) << 8)
#define MM_S1_SH_INNER_SHAREABLE    (_UL(0x3) << 8)

// AP[7:6] Access Permissions
//   0b00: EL1 Read/Write, EL0 No Access
//   0b01: EL1 Read/Write, EL0 Read/Write
//   0b10: EL1 Read-Only, EL0 No Access
//   0b11: EL1 Read-Only, EL0 Read-Only
// ARMv7 だと EL を PL と呼ぶので注意
#define MM_S1_AP_RW_EL1             (_UL(0x0) << 6)
#define MM_S1_AP_RW_ALL             (_UL(0x1) << 6)
#define MM_S1_AP_RO_EL1             (_UL(0x2) << 6)
#define MM_S1_AP_RO_ALL             (_UL(0x3) << 6)

// NS[5] Non-seccure
//   0b0: secure
//   0b1: non-secure
#define MM_S1_NS_SECURE             (_UL(0x0) << 5)
#define MM_S1_NS_NONSECURE          (_UL(0x1) << 5)

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
#define MM_TYPE_PAGE_TABLE          (_UL(0x3) << 0)
#define MM_TYPE_PAGE                (_UL(0x3) << 0)
#define MM_TYPE_BLOCK               (_UL(0x1) << 0)


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
#define MM_S2_RES0                  (_UL(0x0) << 11)

// AF[10] Access flag (Stage 1 と同じ)
#define MM_S2_AF_NO_ACCESS          (_UL(0x0) << 10)
#define MM_S2_AF_ACCESS             (_UL(0x1) << 10)

// SH[9:8] Shareability field (Stage 1 と同じ)
// S1 と S2 の SH の設定を組み合わせて最終的なキャッシュの制御が決まる
#define MM_S2_SH_NON_SHAREABLE      (_UL(0x0) << 8)
#define MM_S2_SH_OUTER_SHAREABLE    (_UL(0x2) << 8)
#define MM_S2_SH_INNER_SHAREABLE    (_UL(0x3) << 8)

// HAP[7:6]: Stage 2 access permissions bits
//   0b00: アクセス禁止
//   0b01: 読み取りのみ許可され、Stage 1 のアクセス許可に関係なく書き込みは許可されない
//   0b10: 書き込みのみ許可され、Stage 1 のアクセス許可に関係なく読み取りは許可されない
//   0b11: 読み書きを許可、つまり Stage 1 のアクセス許可がそのまま適用される
#define MM_S2_HAP_NONE              (_UL(0x0) << 6)
#define MM_S2_HAP_RO                (_UL(0x1) << 6)
#define MM_S2_HAP_WO                (_UL(0x2) << 6)
#define MM_S2_HAP_RW                (_UL(0x3) << 6)

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
#define MM_S2_MEMATTR_STRONGLY_ORDERED  (_UL(0x0) << 2)
#define MM_S2_MEMATTR_DEVICE            (_UL(0x1) << 2)
#define MM_S2_MEMATTR_NORMAL_NC         (_UL(0x5) << 2)
#define MM_S2_MEMATTR_NORMAL_WT         (_UL(0xA) << 2)
#define MM_S2_MEMATTR_NORMAL_WB         (_UL(0xF) << 2)

// Descriptor type[1:0] は Stage 1 と同じなので定義なし


// TCR_EL1: Translation Control Register
//   EL0/1 向けの Stage 1 のアドレス変換を制御するためのレジスタ
//   https://developer.arm.com/documentation/111179/2025-09_ASL1/AArch64-Registers/TCR-EL1--Translation-Control-Register--EL1-
//   以下の定数定義は TCR_EL1, TCR_EL2, VTCR_EL2 で共通
//
// TxSZ: ARMv8 がサポートする64ビットアドレスのうち、無視するビット数を指定する
//   たとえば T0SZ に 16 を指定すると 64 - 16 = 48 ビットのアドレス 256TB だけが有効になる
//   25 を指定すると 64 - 25 = 39 ビットのアドレス 512GB だけが有効になる
//   48ビットの場合はテーブルは4段になるが39ビットの場合はテーブルは3段になり探索が高速化される
//
// T1SZ[21:16]: TTBR1_EL1 で変換されるアドレスのサイズを指定する
// T0SZ[5:0]:   TTBR0_EL1 で変換されるアドレスのサイズを指定する
//
// HV/OS 側で数値を指定する想定だが、TxSZ に入れる値を 0-63 に制限しビットシフトするマクロを提供する
#define TCR_T0SZ(x)             ((_UL(x)) & _UL(0x3f))
#define TCR_T1SZ(x)             (((_UL(x)) & _UL(0x3f)) << 16)

// TGx: Translation Granule size
//   ARMv8 では 3種類のページサイズがサポートされており、それらの中から選択する
//   4KB が最も一般的
//   TG0 と TG1 でインデックスが異なるので注意
// TG1[31:30]: TTBR1_EL1 で変換されるアドレスのページサイズを指定する
//   0b01: 16KB
//   0b10: 4KB
//   0b11: 64KB
// TG0[15:14]: TTBR0_EL1 で変換されるアドレスのページサイズを指定する
//   0b00: 4KB
//   0b01: 64KB
//   0b10: 16KB
#define TCR_TG1_16K             (_UL(0x1) << 30)
#define TCR_TG1_4K              (_UL(0x2) << 30)
#define TCR_TG1_64K             (_UL(0x3) << 30)
#define TCR_TG0_4K              (_UL(0x0) << 14)
#define TCR_TG0_64K             (_UL(0x1) << 14)
#define TCR_TG0_16K             (_UL(0x2) << 14)


// VTCR_EL2: Virtualization Translation Control Register (EL2)
//   https://developer.arm.com/documentation/ddi0601/2024-09/AArch64-Registers/VTCR-EL2--Virtualization-Translation-Control-Register
// VTCR には必ず 1 をセットしないといけないビットがある
#define VTCR_EL2_RES1           ((_UL(0x1) << 31) | (_UL(0x1) << 2))

// NSA[30]: Non-secure stage 2 translation output address space for the Secure EL1&0 translation regime
//   0b0: All stage 2 translation for the Non-secure IPA space of the Secure EL1&0 translation regime access the Secure PA space
//   0b1: All stage 2 translation for the Non-secure IPA space of the Secure EL1&0 translation regime access the Non-secure PA space
#define VTCR_EL2_NSA            (_UL(0x1) << 30)

// NSW[29]: Non-secure stage 2 translation table address space for the Secure EL1&0 translation regime
//   0b0: All stage 2 translation table walks for the Non-secure IPA space of the Secure EL1&0 translation regime are to the Secure PA space.
//   0b1: All stage 2 translation table walks for the Non-secure IPA space of the Secure EL1&0 translation regime are to the Non-secure PA space.
#define VTCR_EL2_NSW            (_UL(0x1) << 29)

// VS[19]: VMID Size
//   0b0: 8-bit VMID
//   0b1: 16-bit VMID
#define VTCR_EL2_VS_8BIT        (_UL(0x0) << 19)
#define VTCR_EL2_VS_16BIT       (_UL(0x1) << 19)

// PS[18:16]: Physical Address Size for stage 2
//   0b000: 32 bits, 4GB.
//   0b001: 36 bits, 64GB.
//   0b010: 40 bits, 1TB.
//   0b011: 42 bits, 4TB.
//   0b100: 44 bits, 16TB.
//   0b101: 48 bits, 256TB.
//   0b110: 52 bits, 4PB.
//   0b111: 56 bits, 64PB. ただし FEAT_D128 が定義されているときのみ
#define VTCR_EL2_PS_32BIT       (_UL(0x0) << 16) // 4GB
#define VTCR_EL2_PS_36BIT       (_UL(0x1) << 16) // 64GB
#define VTCR_EL2_PS_40BIT       (_UL(0x2) << 16) // 1TB
#define VTCR_EL2_PS_42BIT       (_UL(0x3) << 16) // 4TB
#define VTCR_EL2_PS_44BIT       (_UL(0x4) << 16) // 16TB
#define VTCR_EL2_PS_48BIT       (_UL(0x5) << 16) // 256TB
#define VTCR_EL2_PS_52BIT       (_UL(0x6) << 16) // 4PB

// TG0[15:14]: Granule size for the VTTBR_EL2
//   0b00: 4KB.
//   0b01: 64KB.
//   0b10: 16KB.
#define VTCR_EL2_TG0_4K         (_UL(0x0) << 14)
#define VTCR_EL2_TG0_64K        (_UL(0x1) << 14)
#define VTCR_EL2_TG0_16K        (_UL(0x2) << 14)

// SH0[13:12]: Shareability attribute for stage 2 table walks
//   0b00: Non-shareable.
//   0b10: Outer Shareable.
//   0b11: Inner Shareable.
#define VTCR_EL2_SH0_NON        (_UL(0x0) << 12)
#define VTCR_EL2_SH0_OUTER      (_UL(0x2) << 12)
#define VTCR_EL2_SH0_INNER      (_UL(0x3) << 12)

// ORGN0[11:10], IRGN0[9:8]: Cacheability attributes for stage 2 table walks
//   0b00: Normal memory, Outer Non-cacheable.
//   0b01: Normal memory, Outer Write-Back Read-Allocate Write-Allocate Cacheable.
//   0b10: Normal memory, Outer Write-Through Read-Allocate No Write-Allocate Cacheable.
//   0b11: Normal memory, Outer Write-Back Read-Allocate No Write-Allocate Cacheable.
#define VTCR_EL2_ORGN0_NC       (_UL(0x0) << 10)
#define VTCR_EL2_ORGN0_WB_WA    (_UL(0x1) << 10)
#define VTCR_EL2_ORGN0_WT       (_UL(0x2) << 10)
#define VTCR_EL2_ORGN0_WB_nWA   (_UL(0x3) << 10)
#define VTCR_EL2_IRGN0_NC       (_UL(0x0) << 8)
#define VTCR_EL2_IRGN0_WB_WA    (_UL(0x1) << 8)
#define VTCR_EL2_IRGN0_WT       (_UL(0x2) << 8)
#define VTCR_EL2_IRGN0_WB_nWA   (_UL(0x3) << 8)

// SL0[7:6]: Starting level of the stage 2 translation lookup
// Meaning depends on TG0. For TG0=4KB:
//   0b01 and VTCR_EL2.TG0 is 0b00 and VTCR_EL2.SL2 is 0b0, start at level1
//   ...
#define VTCR_EL2_SL0_L2         (_UL(0x0) << 6)
#define VTCR_EL2_SL0_L1         (_UL(0x1) << 6)
#define VTCR_EL2_SL0_L0         (_UL(0x2) << 6)

// T0SZ[5:0]: TCR_T0SZ と同じ
//   サイズは 2^(64 - T0SZ) で計算される
#define VTCR_T0SZ(x)        ((_UL(x)) & _UL(0x3f))

#endif /* _LIB_ARM_MMU_DEF_H */
