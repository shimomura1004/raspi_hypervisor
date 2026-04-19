#ifndef _BOARDS_VIRT_H
#define _BOARDS_VIRT_H

// QEMU Virt Machine Specific Constants (物理アドレス)
// qemu virt board のメモリマップは qemu のソースコードで定義されている
// https://github.com/qemu/qemu/blob/master/hw/arm/virt.c

// QEMU virt では DRAM は 0x40000000 から配置される
#define RAM_BASE                0x40000000UL
#define RAM_SIZE                0x40000000UL
#define VA_START                0x0000000040000000UL

// todo: UART と同様、GIC も別ヘッダに移す
// GIC v2 (Standard for virt machine unless GICv3 is specified)
#define GIC_BASE                0x08000000
#define GIC_DIST_BASE           (GIC_BASE + 0x0000)
#define GIC_CPU_BASE            (GIC_BASE + 0x10000)

// GIC Distributor Registers
#define GICD_CTLR               (GIC_DIST_BASE + 0x000)
#define GICD_TYPER              (GIC_DIST_BASE + 0x004)
#define GICD_ISENABLER          (GIC_DIST_BASE + 0x100)
#define GICD_ICENABLER          (GIC_DIST_BASE + 0x180)
#define GICD_IPRIORITYR         (GIC_DIST_BASE + 0x400)
#define GICD_ITARGETSR          (GIC_DIST_BASE + 0x800)
#define GICD_ICFG               (GIC_DIST_BASE + 0xc00)
#define GICD_CTLR_ENABLE        (1 << 0)

// GICD_ITARGETSR<n>: Interrupt Processor Targets Registers, n = 0 - 254
//   ひとつひとつのレジスタは32ビットで、8ビットずつに区切って使う
//     n 番目のレジスタは割込み ID の 4n から 4n+3 までのターゲットを設定する
//   GICv2 では CPU は8個までとなっているので、割込みを入れる先の CPU をビットで指定する
//   たとえば GICD_ITARGETSR<32> が 0b0000 0001 0000 0011 0000 0111 0000 1111 なら
//    ID 4x32 から 4x32+3 まで、つまり ID 128-131 の割込み先 CPU が指定されていて
//      ID 128 は CPU 0 に送る
//      ID 129 は CPU 0,1 に送る
//      ID 130 は CPU 0,1,2 に送る
//      ID 131 は CPU 0,1,2,3 に送る
//   GICD_ITARGETSR の初期値は実装依存だが 0 である場合が多く、この場合はどの CPU にも配送されないので注意
//   ID 0-31 は SGI/PPI であり CPU 固有の割込みなので読み取り専用である
//   GICD_ITARGETSR は GICv2 のためのもので、GICv3 以降では別の仕組みを使う

// GIC CPU Interface Registers
// PMR: CPU interface priority mask register
//   https://developer.arm.com/documentation/ddi0601/2025-12/External-Registers/GICC-PMR--CPU-Interface-Priority-Mask-Register
//   この値より優先度の高い割込みのみ PE に通知される
//   [7:0] Priority
// IAR: Interrupt Acknowledge Register
//   https://developer.arm.com/documentation/ddi0601/2025-12/External-Registers/GICC-IAR--CPU-Interface-Interrupt-Acknowledge-Register
//   現在発生している割込みのうち優先度が最も高いものの割込み番号を返す
//   割込み番号を読み取ると GIC は自動的にその割込みを処理中とマークし、それより優先度の低い割込みは無視される
// EOIR: End of Interrupt Register
//   https://developer.arm.com/documentation/ihi0048/b/Programmers--Model/CPU-interface-register-descriptions/End-of-Interrupt-Register--GICC-EOIR
//   書き込んだ割込み番号の処理が終了したことを GIC に通知する
#define GICC_CTLR               (GIC_CPU_BASE + 0x000)
#define GICC_PMR                (GIC_CPU_BASE + 0x004)
#define GICC_IAR                (GIC_CPU_BASE + 0x000c)
#define GICC_EOIR               (GIC_CPU_BASE + 0x0010)

#define GICC_CTLR_ENABLE        (1 << 0)

#define GICC_IAR_ID_MASK        0x3ff
#define GIC_IRQ_ID_SPURIOUS     1023
#define GIC_IRQ_ID_NONE         1022

// GIC では割込みは SGI, PPI, SPI, LPI に分かれている
//   SGI: Software Generated Interrupt (0-15)
//     ソフトが生成する割込みで、CPU 間の通信などに使われる   
//   PPI: Private Peripheral Interrupt (16-31)
//     各 CPU 固有の割込みで、タイマ割込みなどに使われる
//   SPI: Shared Peripheral Interrupt  (32-1019)
//     全ての CPU で共有される割込みで、UART などの外部デバイスの割込みに使われる
//   LPI: Local Peripheral Interrupt   (8192-) (GICv3 以降のみ)
//     メッセージベースの割込み
#define IRQ_ID_SGI_START        0
#define IRQ_ID_PPI_START        16
#define IRQ_ID_SPI_START        32

// For abstraction compatibility
// virt の MMIO デバイスは GIC が先頭
#define PERIPHERAL_BASE         GIC_BASE
#define DEVICE_BASE             PERIPHERAL_BASE
#define LOCAL_PERIPHERAL_BASE   0x00000000 // Placeholder for virt

// Use PL011 UART for virt machine
#define UART_TYPE_PL011         1

// QEMU VirtIO MMIO (Block device)
#define VIRTIO_BLK_BASE         0x0a000000

// IRQ Assignments for QEMU virt
#define IRQ_HYP_PHYS_TIMER  26
#define IRQ_VIRT_TIMER      27
#define IRQ_UART0           33

#endif /* _BOARDS_VIRT_H */
