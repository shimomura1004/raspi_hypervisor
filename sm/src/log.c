#include "sm_shmem_log.h"

static sm_shmem_log_t * const log_ptr = (sm_shmem_log_t *)SM_SHARED_MEM_ADDR;

void sm_log_init(void) {
    log_ptr->write_pos = 0;
    log_ptr->read_pos = 0;

    for (int i = 0; i < SM_LOG_BUFFER_SIZE; i++) {
        log_ptr->buffer[i] = 0;
    }
}

void sm_putc(char c) {
    uint32_t current_pos = log_ptr->write_pos;
    uint32_t next_pos = (current_pos + 1) % SM_LOG_BUFFER_SIZE;

    // セキュアモニタを止めないため、バッファが足りなかったら上書き    
    log_ptr->buffer[current_pos] = c;
    
    __asm__ volatile("dmb sy");
    
    log_ptr->write_pos = next_pos;
}

void sm_print(const char *s) {
    while (*s) {
        sm_putc(*s++);
    }
}
