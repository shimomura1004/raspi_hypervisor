#include "block.h"
#include "sd.h"

int block_init(void) {
    return sd_init();
}

int block_readblock(unsigned int lba, unsigned char *buffer, unsigned int num) {
    return sd_readblock(lba, buffer, num);
}
