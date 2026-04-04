#include "block.h"
#include "board_config.h"
#include "debug.h"

#if defined(BOARD_RASPI3)
#include "sd.h"
#elif defined(BOARD_VIRT)
#include "virtio_blk.h"
#endif

int block_init(void) {
#if defined(BOARD_RASPI3)
    return sd_init();
#elif defined(BOARD_VIRT)
    return virtio_blk_init();
#else
    return BLOCK_ERROR;
#endif
}

int block_readblock(unsigned int lba, unsigned char *buffer, unsigned int num) {
#if defined(BOARD_RASPI3)
    return sd_readblock(lba, buffer, num);
#elif defined(BOARD_VIRT)
    return virtio_blk_read(lba, buffer, num);
#else
    return 0; // Error: 0 bytes read
#endif
}
