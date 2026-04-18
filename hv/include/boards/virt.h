#ifndef _BOARDS_VIRT_H
#define _BOARDS_VIRT_H

// QEMU Virt Machine Specific Constants (物理アドレス)
// qemu virt board のメモリマップは qemu のソースコードで定義されている
// https://github.com/qemu/qemu/blob/master/hw/arm/virt.c

// QEMU virt では DRAM は 0x40000000 から配置される
#define RAM_BASE                0x40000000UL
#define RAM_SIZE                0x40000000UL
#define VA_START                0x0000000040000000UL

// GIC v2 (Standard for virt machine unless GICv3 is specified)
// todo: UART と同様、GIC も別ヘッダに移す
#define GIC_BASE                0x08000000
#define GIC_DIST_BASE           (GIC_BASE + 0x0000)
#define GIC_CPU_BASE            (GIC_BASE + 0x10000)

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

#endif /* _BOARDS_VIRT_H */
