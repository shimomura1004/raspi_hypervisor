.PHONY: all clean debug test

ifeq ($(filter clean os-clean hv-clean sm-clean test-clean,$(MAKECMDGOALS)),)
  ifeq ($(BOARD),)
    $(error BOARD environment variable is not defined. Please set it (e.g., export BOARD=raspi3))
  endif
endif

ARMGNU ?= aarch64-linux-gnu

# ログレベルの設定(デフォルトはINFO)
# make 時に変更する場合は make LOG_LEVEL=LOG_LEVEL_DEBUG などとする
LOG_LEVEL ?= LOG_LEVEL_INFO

# ターゲットボードの設定 (デフォルトは raspi3)
BOARD ?= raspi3

COPS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -I../lib/include -mgeneral-regs-only
COPS += -g -O0
COPS += -DLOG_LEVEL=$(LOG_LEVEL)
COPS += -DPRINTF_LONG_SUPPORT
COPS += -DBOARD_$(shell echo $(BOARD) | tr a-z A-Z)
COPS += -DBOARD_NAME=\"$(BOARD)\"

ASMOPS = -Iinclude -I../lib/include -g
ASMOPS += -DBOARD_$(shell echo $(BOARD) | tr a-z A-Z)

BUILD_DIR = build
SRC_DIR = src

all: $(BUILD_DIR)/kernel8.img $(BUILD_DIR)/fs.img

test:
	$(MAKE) -C tests test


clean:
	rm -rf $(BUILD_DIR)
	$(MAKE) -C tests clean

debug: kernel8.img
	gdb-multiarch -ex 'target remote :1234' \
	-ex 'layout asm' \
	-ex 'source ./debug_handler.py'

OS_BINS = \
	../os/echo/build/echo.bin \
	../os/raspios/build/kernel8.img \
	../os/vmm/build/vmm.bin

$(BUILD_DIR)/fs.img: $(OS_BINS)
	mkdir -p $(BUILD_DIR)
	dd if=/dev/zero of=$(BUILD_DIR)/fs.img bs=1M count=64
	mformat -i $(BUILD_DIR)/fs.img -F ::
	-mcopy -i $(BUILD_DIR)/fs.img ../os/echo/build/echo.bin ::ECHO.BIN
	-mcopy -i $(BUILD_DIR)/fs.img ../os/echo/build/kernel8.elf ::ECHO.ELF
	-mcopy -i $(BUILD_DIR)/fs.img ../os/raspios/build/kernel8.img ::RASPIOS.BIN
	-mcopy -i $(BUILD_DIR)/fs.img ../os/raspios/build/kernel8.elf ::RASPIOS.ELF
	-mcopy -i $(BUILD_DIR)/fs.img ../os/vmm/build/kernel8.elf ::VMM.ELF

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

$(BUILD_DIR)/printf_c.o: ../lib/src/printf.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/common_utils_c.o: ../lib/src/common_utils.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/common_utils_s.o: ../lib/src/common_utils.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

$(BUILD_DIR)/boards/$(BOARD)/%_c.o: $(SRC_DIR)/boards/$(BOARD)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/boards/$(BOARD)/%_s.o: $(SRC_DIR)/boards/$(BOARD)/%.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

# 共通のソースファイル
C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)

# ボード固有のソースファイル
BOARD_SRC_DIR = $(SRC_DIR)/boards/$(BOARD)
BOARD_C_FILES = $(wildcard $(BOARD_SRC_DIR)/*.c)
BOARD_ASM_FILES = $(wildcard $(BOARD_SRC_DIR)/*.S)

# オブジェクトファイルの生成
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)
OBJ_FILES += $(BOARD_C_FILES:$(BOARD_SRC_DIR)/%.c=$(BUILD_DIR)/boards/$(BOARD)/%_c.o)
OBJ_FILES += $(BOARD_ASM_FILES:$(BOARD_SRC_DIR)/%.S=$(BUILD_DIR)/boards/$(BOARD)/%_s.o)
OBJ_FILES += $(BUILD_DIR)/printf_c.o
OBJ_FILES += $(BUILD_DIR)/common_utils_c.o
OBJ_FILES += $(BUILD_DIR)/common_utils_s.o

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

# kernel8.img は、kernel8.elf を objcopy で raw binary にしたもの
# raw binary: 実際にメモリに展開される内容がそのままファイルになったもの
#             elf ファイルのようにセクションごとにパースなどをせず全体をコピーするだけでいいので楽
$(BUILD_DIR)/kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES)
	$(ARMGNU)-ld -Map $(BUILD_DIR)/kernel8.map -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf  $(OBJ_FILES)
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary $(BUILD_DIR)/kernel8.img

# elf バイナリを逆アセンブルする
#  aarch64-linux-gnu-objdump -D os/raspios/build/kernel8.elf
# raw binary である kernel8.img を逆アセンブルする
#  aarch64-linux-gnu-objdump -b binary --architecture=aarch64 -D kernel8.img
# raw binary を elf に戻す
#   aarch64-linux-gnu-objcopy -Ibinary -Oelf64-littleaarch64 kernel8.img hoge
# elf を disassemble する (-d ではなく -D を使う)
#   aarch64-linux-gnu-objdump -b binary --target=elf64-littleaarch64 -D hoge
# raw にするとシンボル情報などが落ち、開始・終了位置とサイズだけになる
#   aarch64-linux-gnu-nm hoge

# kernel8.img is a flat binary and has no section information
# use objdmp kernel8.elf to see section names
