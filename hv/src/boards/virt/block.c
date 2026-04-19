#include "block.h"
#include "virtio_blk.h"

int block_init(void) {
    return virtio_blk_init();
}

int block_readblock(unsigned int lba, unsigned char *buffer, unsigned int num) {
    return virtio_blk_read(lba, buffer, num);
}
