#ifndef SM_SHMEM_LOG_H
#define SM_SHMEM_LOG_H

#include <stdint.h>

#define SM_SHARED_MEM_ADDR  0x3b800
#define SM_LOG_BUFFER_SIZE  1024

typedef struct {
    volatile uint32_t write_pos;
    volatile uint32_t read_pos;
    char buffer[SM_LOG_BUFFER_SIZE];
} sm_shmem_log_t;

#endif
