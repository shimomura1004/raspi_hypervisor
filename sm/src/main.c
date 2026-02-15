#include "log.h"

void print_boot_message() {
    sm_print("Secure monitor initialized\r\n");
}

void print_message() {
    sm_print("SMC received\r\n");
}
