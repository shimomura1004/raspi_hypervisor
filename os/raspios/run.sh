#!/bin/bash

# -kernel オプションを使うと el2 から開始できるが、
# イメージをロードするアドレスを指定できないのでフラットバイナリが使えない
#DISPLAY=:0 qemu-system-aarch64 -m 1024 -M raspi3b -kernel kernel8.img -nographic -serial null -serial mon:stdio -s $*

# -kernel オプションを使うと、指定した elf ファイルに埋め込まれた p_paddr を見てロード先を決める
# またそのとき PC の初期値は p_paddr に設定される

BOARD=${BOARD:-raspi3}
KERNEL_IMG=./build/kernel8.img
KERNEL_ELF=./build/kernel8.elf

echo "Run OS on $BOARD"
if [ "$BOARD" = "virt" ]; then
    DISPLAY=:0 qemu-system-aarch64 -m 1024 -M virt,gic-version=2 -cpu cortex-a53 -smp 4 -kernel $KERNEL_ELF -nographic -serial mon:stdio -net none -s $*
else
    DISPLAY=:0 qemu-system-aarch64 -m 1024 -M raspi3b -kernel $KERNEL_ELF -nographic -serial null -serial mon:stdio -s $*
fi
