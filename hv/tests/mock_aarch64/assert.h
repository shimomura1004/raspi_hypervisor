#ifndef _ASSERT_H
#define _ASSERT_H
#include "printf.h"
#define assert(expr) \
    if (!(expr)) { \
        printf("ASSERTION FAILED: %s at %s:%d\n", #expr, __FILE__, __LINE__); \
        while(1); \
    }
#endif
