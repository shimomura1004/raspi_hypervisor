#include "virtio_blk.h"
#include "block.h"
#include "debug.h"

int virtio_blk_init(void) {
    // TODO: Implement VirtIO MMIO initialization
    // For now, return success to allow booting
    INFO("VirtIO block device initialized (stub)");
    return BLOCK_OK;
}

int virtio_blk_read(unsigned int lba, unsigned char *buffer, unsigned int num) {
    // TODO: Implement VirtIO read logic
    WARN("virtio_blk_read: Not implemented yet");
    return 0; // 0 bytes read (error)
}
