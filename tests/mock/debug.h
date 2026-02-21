#ifndef _DEBUG_MOCK_H
#define _DEBUG_MOCK_H

#include <stdio.h>

#define WARN(fmt, ...) printf("WARN: " fmt "\n", ##__VA_ARGS__)

#endif
