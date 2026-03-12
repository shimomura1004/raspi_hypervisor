#include "board_config.h"
#include "generic_timer.h"

/* Virt machine can use the ARM Generic Timer directly.
 * We'll need to route the interrupts via GIC if we want to handle them.
 */
void systimer_init(void) {
    /* Generic timer is initialized in pcpu initialization */
}
