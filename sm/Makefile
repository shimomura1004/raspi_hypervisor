.PHONY: all clean debug

ARMGNU ?= aarch64-linux-gnu

# ログレベルの設定(デフォルトはINFO)
# make 時に変更する場合は make LOG_LEVEL=LOG_LEVEL_DEBUG などとする
LOG_LEVEL ?= LOG_LEVEL_INFO

COPS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only
COPS += -g -O0
COPS += -DLOG_LEVEL=$(LOG_LEVEL)
ASMOPS = -Iinclude -g

BUILD_DIR = build
SRC_DIR = src

all: kernel8.img

clean:
	rm -rf $(BUILD_DIR) *.img 

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

# kernel8.img は、kernel8.elf を objcopy で raw binary にしたもの
# raw binary: 実際にメモリに展開される内容がそのままファイルになったもの
#             elf ファイルのようにセクションごとにパースなどをせず全体をコピーするだけでいいので楽
kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES)
	$(ARMGNU)-ld -Map kernel8.map -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf  $(OBJ_FILES)
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary kernel8.img
