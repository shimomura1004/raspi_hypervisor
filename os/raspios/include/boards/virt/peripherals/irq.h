#ifndef	_P_IRQ_H
#define	_P_IRQ_H

#include "peripherals/base.h"

// GIC v2 Address Map for QEMU virt
#define GIC_BASE                PBASE
#define GIC_DIST_BASE           (GIC_BASE + 0x00000)
#define GIC_CPU_BASE            (GIC_BASE + 0x10000)

/* GICv2 Registers */
#define GICD_CTLR       (GIC_DIST_BASE + 0x000)
#define GICD_TYPER      (GIC_DIST_BASE + 0x004)
#define GICD_ISENABLER  (GIC_DIST_BASE + 0x100)
#define GICD_ICENABLER  (GIC_DIST_BASE + 0x180)
#define GICD_IPRIORITYR (GIC_DIST_BASE + 0x400)
#define GICD_ITARGETSR  (GIC_DIST_BASE + 0x800)
#define GICD_ICFG       (GIC_DIST_BASE + 0xc00)

#define GICC_CTLR       (GIC_CPU_BASE + 0x000)
#define GICC_PMR        (GIC_CPU_BASE + 0x004)
#define GICC_IAR        (GIC_CPU_BASE + 0x00c)
#define GICC_EOIR       (GIC_CPU_BASE + 0x010)

void clear_timer_irq(void);

#endif  /*_P_IRQ_H */
