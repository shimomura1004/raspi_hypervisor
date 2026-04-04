#ifndef _BLOCK_H
#define _BLOCK_H

#define BLOCK_OK    0
#define BLOCK_ERROR -1

int block_init(void);
int block_readblock(unsigned int lba, unsigned char *buffer, unsigned int num);

#endif
