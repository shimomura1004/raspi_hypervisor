#ifndef _BOARDS_VIRT_H
#define _BOARDS_VIRT_H

// QEMU Virt Machine Specific Constants (物理アドレス)

// QEMU では DRAM は 0x40000000 から配置される
#define RAM_BASE                0x40000000UL
#define RAM_SIZE                0x40000000UL
#define VA_START                0x0000000040000000UL

// QEMU Virt UART0 (PL011)
#define UART_BASE               0x09000000

// GIC v2 (Standard for virt machine unless GICv3 is specified)
#define GIC_BASE                0x08000000
#define GIC_DIST_BASE           (GIC_BASE + 0x0000)
#define GIC_CPU_BASE            (GIC_BASE + 0x10000)

// QEMU VirtIO MMIO (Block device)
#define VIRTIO_BLK_BASE         0x0a000000

// For abstraction compatibility
#define PERIPHERAL_BASE         GIC_BASE
#define DEVICE_BASE             PERIPHERAL_BASE
#define LOCAL_PERIPHERAL_BASE   0x00000000 // Placeholder for virt

#define UART_TYPE_PL011         1

// VMM(ELF)の初期スタックポインタ
#define VMM_DEFAULT_STACK_ADDR  (RAM_BASE + 0x200000)

#endif /* _BOARDS_VIRT_H */
