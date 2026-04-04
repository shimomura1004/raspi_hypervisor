#ifndef _VIRTIO_BLK_H
#define _VIRTIO_BLK_H

int virtio_blk_init(void);
int virtio_blk_read(unsigned int lba, unsigned char *buffer, unsigned int num);

#endif
