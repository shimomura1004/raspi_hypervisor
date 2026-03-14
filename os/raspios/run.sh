#!/bin/bash

# -kernel オプションを使うと el2 から開始できるが、
# イメージをロードするアドレスを指定できないのでフラットバイナリが使えない
#DISPLAY=:0 qemu-system-aarch64 -m 1024 -M raspi3b -kernel kernel8.img -nographic -serial null -serial mon:stdio -s $*

BOARD=${BOARD:-raspi3}
KERNEL_IMG=./build/kernel8.img

echo "Run OS on $BOARD"
if [ "$BOARD" = "virt" ]; then
    DISPLAY=:0 qemu-system-aarch64 -m 1024 -M virt,gic-version=2 -cpu cortex-a53 -smp 4 -device loader,file=$KERNEL_IMG,addr=0x40100000,cpu-num=0 -nographic -serial mon:stdio -net none -s $*
else
    DISPLAY=:0 qemu-system-aarch64 -m 1024 -M raspi3b -device loader,file=$KERNEL_IMG,addr=0x0 -nographic -serial null -serial mon:stdio -s $*
fi
