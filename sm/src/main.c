#include "log.h"

void print_boot_message() {
    sm_print("Secure monitor initialized\r\n");
}

void print_message(unsigned long x0, unsigned long x1, unsigned long x2, unsigned long x3, unsigned long x4) {
    sm_print("SMC received\r\n");
}
