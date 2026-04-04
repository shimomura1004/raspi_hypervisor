#ifndef _BOARDS_VIRT_H
#define _BOARDS_VIRT_H

// QEMU Virt Machine Specific Constants

#define RAM_BASE                0x40000000
#define RAM_SIZE                0x40000000      // 1GB default for now
#define VA_START                0x0000000040000000

// QEMU Virt UART0 (PL011)
#define UART_BASE               (VA_START + 0x09000000)

// GIC v2 (Standard for virt machine unless GICv3 is specified)
#define GIC_BASE                (VA_START + 0x08000000)
#define GIC_DIST_BASE           (GIC_BASE + 0x0000)
#define GIC_CPU_BASE            (GIC_BASE + 0x10000)

// For abstraction compatibility
#define PERIPHERAL_BASE         GIC_BASE
#define LOCAL_PERIPHERAL_BASE   0x00000000 // Placeholder for virt

#define UART_TYPE_PL011         1

#endif /* _BOARDS_VIRT_H */
