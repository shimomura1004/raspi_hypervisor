#ifndef _BOARDS_VIRT_H
#define _BOARDS_VIRT_H

// QEMU Virt Machine Specific Constants (物理アドレス)
// qemu virt board のメモリマップは qemu のソースコードで定義されている
// https://github.com/qemu/qemu/blob/master/hw/arm/virt.c
#define VA_START                0x0000000040000000UL

// 利用可能な物理メモリの終端
// lib/include/boards/virt/peripherals/base.h で定義される RAM_BASE と RAM_SIZE が使用したいが
// ただし base.h -> mm.h -> board_config.h -> virt.h の循環参照があるため
// リテラルで定義するかインクルードの順序を工夫する必要がある
// todo: いったんリテラルで定義
#define HIGH_MEMORY             (0x40000000UL + 0x40000000UL)

#include "peripherals/base.h"
#include "peripherals/irq.h"

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
