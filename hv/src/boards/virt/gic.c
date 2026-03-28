#include "utils.h"
#include "board_config.h"
#include "generic_timer.h"
#include "irq.h"
#include <inttypes.h>

/* GICv2 Registers (relative to GIC_DIST_BASE and GIC_CPU_BASE) */
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

void enable_local_interrupt_controller(unsigned long cpuid) {
    /* Enable GIC CPU interface */
    put32(GICC_PMR, 0xff);    /* Allow all priority levels */
    put32(GICC_CTLR, 0x1);    /* Enable Group 0 (or Group 1 if GICv2) */
}

void enable_legacy_interrupt_controller(void) {
    /* Global GIC Distributor initialization */
    put32(GICD_CTLR, 0x1);    /* Enable Distributor */
}

/* Note: handle_irq will be called from vectors. 
 * For GIC, we need to read IAR to get the IRQ number and write EOIR when done.
 */
void handle_irq(void) {
    unsigned int iar = get32(GICC_IAR);
    unsigned int irq = iar & 0x3ff;

    if (irq == IRQ_HYP_PHYS_TIMER) {
        handle_generic_timer_irq();
    } else if (irq == IRQ_VIRT_TIMER) {
        handle_virtual_timer_irq();
    } else if (irq < 1022) {
        // その他の IRQ (UART など) は今は無視
    }

        put32(GICC_EOIR, iar);
    }

void enable_virtual_timer_irq(void) {
    put32(GICD_ISENABLER, (1 << IRQ_VIRT_TIMER));
}

void disable_virtual_timer_irq(void) {
    put32(GICD_ICENABLER, (1 << IRQ_VIRT_TIMER));
}
