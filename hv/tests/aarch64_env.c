#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include "printf.h"
#include "spinlock.h"

// Define malloc mock for tests
static char heap[1024 * 1024]; // 1MB heap
static size_t heap_ptr = 0;

void *malloc(size_t size) {
    if (heap_ptr + size > sizeof(heap)) return NULL;
    void *ptr = &heap[heap_ptr];
    heap_ptr += (size + 7) & ~7; // 8-byte alignment
    return ptr;
}

void free(void *ptr) {
    (void)ptr;
}

__attribute__((weak)) unsigned long allocate_page() {
    return (unsigned long)malloc(4096);
}

__attribute__((weak)) void free_page(void *p) {
    free(p);
}

#include "spinlock.h" // Should pick mock/spinlock.h
#define _SPINLOCK_H   // Prevent ../include/spinlock.h from being included
#include "cpu_core.h"
#include "sched.h"

// Spinlock mocks are provided as static inline in mock/spinlock.h

// Mock variables and functions for bare-metal test environment
struct pcpu_struct mock_pcpu;
struct pcpu_struct* current_pcpu() { return &mock_pcpu; }

void flush_vm_console(struct vm_struct2 *vm) { (void)vm; }
void show_vm_list() {}
void show_systimer_info() {}
struct vm_struct2 *vms2[NUMBER_OF_VMS];
struct vcpu_struct *vcpus[NUMBER_OF_VCPUS];

// interrupt-related mocks
int is_interrupt_enabled() { return 0; }
void disable_irq() {}
void enable_irq() {}
void exit_vm() { while(1); }

// System-wide log levels etc. (Passed via -DLOG_LEVEL=...)
// Required character output function for printf.c, implemented in mini_uart.c
extern void putc(void *p, char c);
extern void uart_init();

// The test's main function (will be renamed from 'main' via -Dmain=test_main)
extern int test_main();

// QEMU Semihosting exit
void qemu_exit(int code) {
    // Parameter block for SYS_EXIT (0x18)
    // Word 0: reason (0x20026 = ADP_Stopped_ApplicationExit)
    // Word 1: exit code
    volatile unsigned long block[2];
    block[0] = 0x20026;
    block[1] = code;

    register unsigned long x0 asm("x0") = 0x18;
    register unsigned long x1 asm("x1") = (unsigned long)block;

    asm volatile (
        "hlt #0xf000"
        :
        : "r"(x0), "r"(x1)
        : "memory"
    );
}

void aarch64_main() {
    uart_init();
    init_printf(NULL, putc);
    
    // Call the test main
    test_main();
    
    tfp_printf("\n[AARCH64 TEST RUNNER] Test execution completed successfully.\n");
    qemu_exit(0);
}

// Handle abort()
void abort() {
    tfp_printf("ABORT called\n");
    qemu_exit(1);
}
