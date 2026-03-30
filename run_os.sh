#!/bin/bash

BOARD=${BOARD:-raspi3}
KERNEL_IMG=./os/raspios/build/kernel8.img
KERNEL_ELF=./os/raspios/build/kernel8.elf

echo "Run OS on $BOARD"
if [ "$BOARD" = "virt" ]; then
    DISPLAY=:0 qemu-system-aarch64 -m 1024 -M virt,gic-version=2 -cpu cortex-a53 -smp 4 -kernel $KERNEL_ELF -nographic -serial mon:stdio -net none -s $*
else
    DISPLAY=:0 qemu-system-aarch64 -m 1024 -M raspi3b -kernel $KERNEL_ELF -nographic -serial null -serial mon:stdio -s $*
fi
