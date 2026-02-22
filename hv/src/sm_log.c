#include <stddef.h>
#include "sm_log.h"
#include "mm.h"
#include "mini_uart.h"
#include "debug.h"

// 共有メモリへのポインタ
// ハイパーバイザはリニアマッピングされているため、物理アドレス + VA_START でアクセス可能
static sm_shmem_log_t * const log_ptr = (sm_shmem_log_t *)(SM_SHARED_MEM_ADDR + VA_START);

void sm_log_dump(void) {
    uint32_t write_pos = log_ptr->write_pos;
    uint32_t read_pos = log_ptr->read_pos;

    // バッファインデックスの健全性チェック
    if (write_pos >= SM_LOG_BUFFER_SIZE || read_pos >= SM_LOG_BUFFER_SIZE) {
        WARN("Secure monitor log index error: w/r = %d/%d", write_pos, read_pos);
        return;
    }

    // バッファが空であればなにもせず戻る
    if (read_pos == write_pos) {
        return;
    }

    // todo: ロックも取らないといけないし、その他のログと統一したほうがいいかも
    acquire_lock(&console_lock);
    printf("%15s ", "\x1b[32m" "SM" "\x1b[39m");

    while (read_pos != write_pos) {
        putc(NULL, log_ptr->buffer[read_pos]);
        read_pos = (read_pos + 1) % SM_LOG_BUFFER_SIZE;
    }
    release_lock(&console_lock);

    log_ptr->read_pos = read_pos;
}
