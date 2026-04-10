#ifndef _DEBUG_H
#define _DEBUG_H

#include "printf.h"
#include "irq.h"
#include "drivers/uart.h"
#include "spinlock.h"

extern struct spinlock console_lock;

#define _LOG_COMMON(level, fmt, ...) do { \
    acquire_lock(&console_lock); \
    unsigned long cpuid = get_cpuid(); \
    printf("<cpu:%d> %s: ", cpuid, level); \
    printf(fmt "\n", ##__VA_ARGS__); \
    release_lock(&console_lock); \
} while (0)

#define INFO(fmt, ...) _LOG_COMMON("INFO", fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) _LOG_COMMON("WARN", fmt, ##__VA_ARGS__)

// panic 後も割り込みが入ると普通に動いてしまうので割り込みを禁止する
#define PANIC(fmt, ...) do { \
    _LOG_COMMON("PANIC", fmt, ##__VA_ARGS__); \
    disable_irq(); \
    while(1); \
} while(0)

#endif
