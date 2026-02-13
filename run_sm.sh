#!/bin/bash

DISPLAY=:0 qemu-system-aarch64 -m 1024 -M raspi3b -device loader,file=./sm/kernel8.img,addr=0x0 -device loader,file=./hv/kernel8.img,addr=0x80000 -nographic -serial null -serial mon:stdio -drive format=raw,file=./hv/fs.img -s $*
