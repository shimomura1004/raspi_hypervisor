#ifndef _DEBUG_MOCK_H
#define _DEBUG_MOCK_H

#include <stdio.h>

#define WARN(fmt, ...) printf("WARN: " fmt "\n", ##__VA_ARGS__)
#define INFO(fmt, ...) printf("INFO: " fmt "\n", ##__VA_ARGS__)
#define DEBUG(fmt, ...) printf("DEBUG: " fmt "\n", ##__VA_ARGS__)
#define PANIC(fmt, ...) do { printf("PANIC: " fmt "\n", ##__VA_ARGS__); while(1); } while(0)

#endif
