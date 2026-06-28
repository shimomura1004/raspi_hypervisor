#ifndef _BOARDS_VIRT_H
#define _BOARDS_VIRT_H

// ハイパーバイザが動作する仮想アドレスと virt の物理アドレスのマッピング
//   - VA: 0x0000_0000-0x3fff_ffff (Device region)
//       - PA: 0x0000_0000-0x3fff_ffff -> VA: 0x0000_0000-0x3fff_ffff
//   - VA: 0x4000_0000-            (RAM)
//       - PA: 0x4000_0000-            -> VA: 0x4000_0000-

// virt ボード用の物理アドレス・仮想アドレスの変換マクロ
//   virt ボードの物理アドレスとハイパーバイザの仮想アドレスは恒等マッピング
#define P2V_RAM(pa)         (pa)

#define P2V(pa)             (pa)
#define V2P(va)             (va)

// QEMU Virt Machine Specific Constants (物理アドレス)
// qemu virt board のメモリマップは qemu のソースコードで定義されている
// https://github.com/qemu/qemu/blob/master/hw/arm/virt.c
// RAM_SIZE は QEMU の設定次第

#define PA_RAM_BASE         0x40000000UL
#define RAM_SIZE            0x40000000UL

// 利用可能な物理メモリの終端
#define HIGH_MEMORY         (PA_RAM_BASE + RAM_SIZE)

// virt ボードではペリフェラルはひとつのブロックに配置されている
// todo: PA_PERIPHERAL_BASE と PA_DEVICE_BASE を分けて定義する意味はない？
#define PA_PERIPHERAL_BASE  0x00000000
#define PA_DEVICE_BASE      PA_PERIPHERAL_BASE
#define DEVICE_SIZE         0x40000000

// Use PL011 UART for virt machine
#define UART_TYPE_PL011         1

// QEMU VirtIO MMIO (Block device)
#define VIRTIO_BLK_BASE     0x0a000000

// IRQ Assignments for QEMU virt
#define IRQ_HYP_PHYS_TIMER  26
#define IRQ_VIRT_TIMER      27
#define IRQ_UART0           33

#endif /* _BOARDS_VIRT_H */
