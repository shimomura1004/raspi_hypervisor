#ifndef _ASSERT_H
#define _ASSERT_H
#include "printf.h"
extern void qemu_exit(int code);

#define assert(expr) \
    if (!(expr)) { \
        printf("ASSERTION FAILED: %s at %s:%d\n", #expr, __FILE__, __LINE__); \
        qemu_exit(1); \
    }
#endif
