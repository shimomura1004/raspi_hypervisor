ifeq ($(filter clean,$(MAKECMDGOALS)),)
  ifeq ($(BOARD),)
    $(error BOARD environment variable is not defined. Please set it (e.g., export BOARD=raspi3))
  endif
endif

ARMGNU ?= aarch64-linux-gnu

BOARD ?= raspi3

COPS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -I../../lib/include -mgeneral-regs-only
COPS += -g -O0
COPS += -DPRINTF_LONG_SUPPORT
COPS += -DBOARD_$(shell echo $(BOARD) | tr a-z A-Z)
COPS += -DBOARD_NAME=\"$(BOARD)\"
COPS += -Iinclude/boards/$(BOARD)
COPS += -I../../lib/include/boards/$(BOARD)

ASMOPS = -Iinclude -I../../lib/include -g -Iinclude/boards/$(BOARD)
ASMOPS += -I../../lib/include/boards/$(BOARD)
ASMOPS += -DBOARD_$(shell echo $(BOARD) | tr a-z A-Z)

UART_DRV = $(if $(filter raspi3,$(BOARD)),mini_uart,pl011_uart)

BUILD_DIR = build
SRC_DIR = src
LINKER_SCRIPT = $(SRC_DIR)/boards/$(BOARD)/linker.ld

ifeq ($(filter clean,$(MAKECMDGOALS)),)
  ifeq ($(wildcard $(LINKER_SCRIPT)),)
    $(error Linker script not found for BOARD=$(BOARD) at $(LINKER_SCRIPT))
  endif
endif

all : $(BUILD_DIR)/kernel8.img

clean :
	rm -rf $(BUILD_DIR)

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

LIB_SRC_DIR = ../../lib/src
# 使用するライブラリソースのリスト
LIB_SOURCES = printf.c common_utils.c common_utils.S smccc.S psci.c spinlock.c spinlock.S arm/irq.S

$(BUILD_DIR)/lib/%_c.o: $(LIB_SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/lib/%_s.o: $(LIB_SRC_DIR)/%.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

$(BUILD_DIR)/lib/drivers/uart/%_c.o: $(LIB_SRC_DIR)/drivers/uart/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)

BOARD_SRC_DIR = $(SRC_DIR)/boards/$(BOARD)
BOARD_C_FILES = $(wildcard $(BOARD_SRC_DIR)/*.c)
BOARD_ASM_FILES = $(wildcard $(BOARD_SRC_DIR)/*.S)

$(BUILD_DIR)/boards/$(BOARD)/%_c.o: $(BOARD_SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/boards/$(BOARD)/%_s.o: $(BOARD_SRC_DIR)/%.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@


OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)
OBJ_FILES += $(BOARD_C_FILES:$(BOARD_SRC_DIR)/%.c=$(BUILD_DIR)/boards/$(BOARD)/%_c.o)
OBJ_FILES += $(BOARD_ASM_FILES:$(BOARD_SRC_DIR)/%.S=$(BUILD_DIR)/boards/$(BOARD)/%_s.o)
OBJ_FILES += $(patsubst %.c,$(BUILD_DIR)/lib/%_c.o,$(filter %.c,$(LIB_SOURCES)))
OBJ_FILES += $(patsubst %.S,$(BUILD_DIR)/lib/%_s.o,$(filter %.S,$(LIB_SOURCES)))
OBJ_FILES += $(BUILD_DIR)/lib/drivers/uart/$(UART_DRV)_c.o

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

$(BUILD_DIR)/kernel8.img: $(LINKER_SCRIPT) $(OBJ_FILES)
	$(ARMGNU)-ld -Map $(BUILD_DIR)/kernel8.map -T $(LINKER_SCRIPT) -o $(BUILD_DIR)/kernel8.elf  $(OBJ_FILES)
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary $(BUILD_DIR)/kernel8.img
