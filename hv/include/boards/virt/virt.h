#ifndef _BOARDS_VIRT_H
#define _BOARDS_VIRT_H

#include "peripherals/base.h"
#include "peripherals/irq.h"

// QEMU Virt Machine Specific Constants (物理アドレス)
// qemu virt board のメモリマップは qemu のソースコードで定義されている
// https://github.com/qemu/qemu/blob/master/hw/arm/virt.c

#define VA_START                0x0000000040000000UL

// For abstraction compatibility
// virt の MMIO デバイスは GIC が先頭
#define PERIPHERAL_BASE         GIC_BASE
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
