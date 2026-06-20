#include "arm/mmu.h"
#include "arm/sysregs_def.h"
#include "mm.h"
#include "board_config.h"

.section ".text.boot"

.extern initialized

// todo: virt 環境で動かない

// todo: 動作確認のため、ハイパーバイザ有無に関わらず同一ソースで起動できるようにしたい
// qemu を -M raspi3b で起動すると EL2 から起動されるため、ネイティブとゲストのソースに差がでてしまう
// 起動後に EL を見て処理を切り替えれば暫定対応できる

//======== create_pgd_pud_entry ===============================================
.macro  create_pgd_pud_entry, tbl, virt, tmp1, tmp2
    // for PGD (PGD は1エントリで 512GB のアドレス空間全体をカバーする)
    add \tmp2, \tbl, #PAGE_SIZE                             // PUD のアドレスを計算 (PUD は PGD のすぐ後ろ)
    create_table_entry \tbl, \virt, PGD_SHIFT, \tmp2, \tmp1 // create PGD entry
    mov \tbl, \tmp2                                         // PUD のアドレスをセット

    // for PUD (PUD は1エントリで1GBをカバーする)
    add \tmp2, \tbl, #PAGE_SIZE                             // PMD のアドレスを計算
    create_table_entry \tbl, \virt, PUD_SHIFT, \tmp2, \tmp1 // create PUD entry
    mov \tbl, \tmp2                                         // PMD のアドレスをセット
.endm

//======== create_table_entry ===============================================
// PUD, PMD のエントリを、上位のテーブルに追記するマクロ
.macro  create_table_entry, tbl, virt, shift, next_tbl, tmp1
    // 割当てる仮想アドレスを右シフトしてインデックス部分を一番右に寄せる
    lsr \tmp1, \virt, #\shift
    // ページサイズのビット幅で and を取り、インデックスに変換(0~511)
    and \tmp1, \tmp1, #PTRS_PER_TABLE - 1
    // ひとつのエントリのサイズは8バイトなので、インデックスを8倍(lsl #3)してオフセットに変換してから足す
    add \tmp1, \tbl, \tmp1, lsl #3
    // ページの情報をアドレスの下位ビットにセット
    orr \tbl, \next_tbl, #MM_TYPE_PAGE_TABLE
    // メモリに書き込む
    str \tbl, [\tmp1]
.endm

//======== create_block_map ===============================================
// 指定された PMD に対して最大512個のエントリ(実ページへのポインタ)を書き込む
// start と end で指定された範囲を tbl で指定されたテーブルに一気に追記していく
// raspios では 4KB ごとのエントリを作成しておらず、2MB ごとの Section Mapping になっている
// よって PMD の各エントリは OS やアプリが使うメモリ領域(セクション)へのポインタになっている
// phys:    マップ元の物理アドレスの開始位置
// start:   マップ先の仮想アドレスの開始位置
// end:     マップ先の仮想アドレスの終了位置
.macro  create_block_map, tbl, phys, start, end, flags, tmp1
    // 開始アドレスと終了アドレスをテーブル内のインデックスに変換
    // ページは 2MB 単位なので SECTION_SHIFT 分シフトしてマスクする
    lsr \start, \start, #SECTION_SHIFT
    and \start, \start, #PTRS_PER_TABLE - 1
    lsr \end, \end, #SECTION_SHIFT
    and \end, \end, #PTRS_PER_TABLE - 1
    // マップする物理アドレスをシフトしてから戻し、ページの先頭アドレスを作る
    // そのときに flags をセットして phys にテーブルエントリを作る(ページ属性は全て同じ)
    lsr \phys, \phys, #SECTION_SHIFT
    mov \tmp1, #\flags
    orr \phys, \tmp1, \phys, lsl #SECTION_SHIFT
9999:
    // テーブルエントリを登録
    str \phys, [\tbl, \start, lsl #3]
    // インデックスを1つ進める
    add \start, \start, #1
    // 物理アドレスのほうは1ページ分(2MB)進める
    add \phys, \phys, #SECTION_SIZE
    // end を超えたらループを抜ける、つまり start <= end の間はループする
    // よって end で指定されたアドレスはマップの対象となる
    cmp \start, \end
    b.ls    9999b
.endm

//======== entry point ===============================================
.globl _start
_start:
    mrs x0, mpidr_el1
    and x0, x0,#0xFF        // Check processor id

    cmp x0, #0
    b.eq el3_entry          // Primary CPU

loop:
    // raspi の場合は PSCI がなく電源投入後に自動で全 CPU コアが起動する
    // CPU0 以外はここでループして initialized 変数がセットされるのを待つ

    // この OS はリンカスクリプトで指定された値によって 0xffff_0000_0000_0000 に配置される
    // initialized も高位アドレスに配置されているため、そのまま ldr x1, [x0] はエラーになる
    //   ldr で initialized 変数の絶対アドレスを直接参照するとエラーになってしまう
    // adrp は PC からの相対距離でアドレスを計算するので、物理アドレスでも仮想アドレスでも正しいアドレスを計算できる
    //   adrp (address of page, pc-relative) は、指定したシンボルのページのアドレスを返す
    //   :lo12: (low 12 bits) はページ内のオフセットを返すので、これらを足すと正しいアドレスになる
    // RAM_BASE が 0 以外（virt ボードなど）の場合でも正しく動作する
    adrp x0, initialized
    add x0, x0, :lo12:initialized
    ldr x1, [x0]
    cbz x1, loop
    dsb ish
    b el3_entry

el3_entry:
    bl get_el
    cmp x0, #1
    b.eq el1_entry

    // EL3 から起動した場合(この OS がベアメタルで起動した場合)は最低限の設定だけして EL1 に切り替える
    ldr x0, =SCTLR_EL2_VALUE_MMU_DISABLED
    msr sctlr_el2, x0

    ldr x0, =HCR_EL2_VALUE_FOR_OS
    msr hcr_el2, x0

    ldr x0, =SCR_EL3_VALUE_FOR_OS
    msr scr_el3, x0

    // SPSR の M ビットを EL1h(0b101) に設定することで EL1 に直接遷移させる
    ldr x0, =SPSR_EL3_VALUE_FOR_OS
    msr spsr_el3, x0

    adr x0, el1_entry
    msr elr_el3, x0

    eret

el1_entry:
    // コア0でのみページテーブルの設定を実行する
    mrs x0, mpidr_el1
    and x0, x0, #0xFF
    cbz x0, page_table_setup
    b after_page_table_setup

page_table_setup:
    // MMU のキャッシュの無効化
    ldr x0, =SCTLR_EL1_VALUE_MMU_DISABLED
    msr sctlr_el1, x0

    // BSS セクションをゼロクリア
    adr x0, bss_begin
    adr x1, bss_end
    sub x1, x1, x0
    bl  memzero_without_stack

    // ページテーブルの作成
    bl  __create_page_tables
    b after_page_table_setup

after_page_table_setup:
    // 各 CPU のスタックポインタを設定する
    // 1コアあたり 4MB (0x400000) のスタック領域を割り当てる
    mov x1, #STACK_SIZE
    bl get_cpuid
    add x0, x0, #1
    mul x0, x1, x0
    // todo: RAM の先頭を使ってもいいのか？最初のあたりは device tree 用になっていたのでは？
    ldr x1, =(VA_START + PA_RAM_BASE)
    add sp, x0, x1

    adrp    x0, pg_dir
    msr ttbr1_el1, x0
    // 最も大事
    adrp    x0, pg_dir
    msr ttbr0_el1, x0

    ldr x0, =(TCR_VALUE)
    msr tcr_el1, x0

    ldr x0, =(MAIR_VALUE)
    msr mair_el1, x0

    // todo: objcopy はコード中のジャンプ先を修正しないため、kernel_main のアドレスがずれる
    ldr x2, =kernel_main

    mov x0, #SCTLR_EL1_MMU_ENABLED

    dsb ish
    isb
    msr sctlr_el1, x0
    isb

    br  x2

// todo: ページの初期化を汎用的にする
// todo: PGD, PUD の初期化の処理がベタ書きになっているのでマクロにするなどする
//   各ページは物理アドレス上で
//   pg_dir
//   |
//   v
//   [PGD], [PUD], [PMD1], [PMD2]
//   というならびになっている
__create_page_tables:
    // 戻りアドレスを控える
    mov x29, x30

    // ページテーブル用の領域全体(PGD 1個, PUD 1個, PMD 2個)をまずゼロクリア
    adrp    x0, pg_dir
    mov x1, #(4 * PAGE_SIZE)
    bl  memzero_without_stack

    // PGD と PUD を設定 (PGD -> PUD -> PMD0 までのリンクを作成)
    adrp    x0, pg_dir
    ldr x1, =VA_START
    create_pgd_pud_entry x0, x1, x2, x3 // x0 は PMD0 (pg_dir + 2*PAGE) を指して戻る

    // PGD[0] に PUD へのリンクを張る (仮想アドレスを有効にした直後にクラッシュしないようにするため)
    //   既に VA_START に対して作成した PMD へのリンクを PGD の最初の PUD へのエントリ(0-1GB用)にも追加する
    // PGD[0x0000] --+
    //               |           ← このリンクを追加する
    // PGD[0xXXXX] --+---> PUD
    adrp x1, pg_dir                 // x1 = PGD の先頭
    add x2, x1, #PAGE_SIZE          // x2 = PUD の物理アドレス
    orr x2, x2, #MM_TYPE_PAGE_TABLE // フラグを追加
    str x2, [x1]                    // PGD のエントリを更新

    // todo: 全体的に似た処理が多いのでマクロを導入したい
    // todo: この定数が定義されているかで分岐するより、ボードごとにファイルをわけたほうがいいのではないか

#if defined(BOARD_RASPI3)
    // PMD0 (0-1GB: DRAM + BCM Peripherals)
    ldr x1, =PA_RAM_BASE
    ldr x2, =(VA_START + PA_RAM_BASE)
    ldr x3, =(VA_START + PA_RAM_BASE + RAM_SIZE - SECTION_SIZE)
    create_block_map x0, x1, x2, x3, MMU_FLAGS, x4

    ldr x1, =PA_DEVICE_BASE
    ldr x2, =(VA_START + PA_DEVICE_BASE)
    ldr x3, =(VA_START + PA_DEVICE_BASE + DEVICE_SIZE - SECTION_SIZE)
    create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4

    // PMD1 (1-2GB: Local Peripherals)
    // PUD に PMD1 へのエントリを追加する
    adrp x2, pg_dir
    add x2, x2, #PAGE_SIZE          // x2 = PUD
    ldr x1, =(VA_START + PA_LOCAL_PERIPHERAL_BASE)
    lsr x1, x1, #PUD_SHIFT
    and x1, x1, #PTRS_PER_TABLE - 1 // インデックス計算
    add x3, x0, #PAGE_SIZE          // x3 = PMD1 (x0 は PMD0 を指している)
    orr x3, x3, #MM_TYPE_PAGE_TABLE
    str x3, [x2, x1, lsl #3]

    add x0, x0, #PAGE_SIZE          // x0 を PMD1 のアドレスに進める
    ldr x1, =PA_LOCAL_PERIPHERAL_BASE
    ldr x2, =(VA_START + PA_LOCAL_PERIPHERAL_BASE)
    ldr x3, =(VA_START + PA_LOCAL_PERIPHERAL_BASE + LOCAL_PERIPHERAL_SIZE - SECTION_SIZE)
    create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4

#elif defined(BOARD_VIRT)
    // PMD0 (0-1GB: Device 領域)
    ldr x1, =PA_DEVICE_BASE
    ldr x2, =(VA_START + PA_DEVICE_BASE)
    ldr x3, =(VA_START + PA_DEVICE_BASE + DEVICE_SIZE - SECTION_SIZE)
    create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4

    // PMD1 (1-2GB: RAM 領域)
    // PUD に PMD1 へのエントリを追加する
    adrp x2, pg_dir
    add x2, x2, #PAGE_SIZE          // x2 = PUD
    ldr x1, =(VA_START + PA_RAM_BASE)  // 物理アドレス上の RAM の位置を加算することでオフセットマッピングする
    lsr x1, x1, #PUD_SHIFT
    and x1, x1, #PTRS_PER_TABLE - 1
    add x3, x0, #PAGE_SIZE          // x3 = PMD1
    orr x3, x3, #MM_TYPE_PAGE_TABLE
    str x3, [x2, x1, lsl #3]

    add x0, x0, #PAGE_SIZE          // x0 を PMD1 のアドレスに進める
    ldr x1, =PA_RAM_BASE
    ldr x2, =(VA_START + PA_RAM_BASE)
    ldr x3, =(VA_START + PA_RAM_BASE + RAM_SIZE - SECTION_SIZE)
    create_block_map x0, x1, x2, x3, MMU_FLAGS, x4
#endif

    // 戻りアドレスを戻して ret する
    mov x30, x29
    ret
