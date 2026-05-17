#ifndef _BCM2836_ARMCTRL_H
#define _BCM2836_ARMCTRL_H

// todo: 周辺機器ごとに用意した方がいいかも
void init_bcm2836_armctrl(unsigned long base_virt_address);

void enable_generic_timer(unsigned long cpuid);
void disable_generic_timer(unsigned long cpuid);
void enable_systimer();
void disable_systimer();
void enable_uart();
void disable_uart();

#endif
