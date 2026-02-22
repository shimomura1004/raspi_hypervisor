BUILD_DIR = build
AARCH64_BUILD_DIR = build_aarch64

# Host tools
CC = gcc
CFLAGS = -I./mock -I../include -Wall -Wextra -g -fno-builtin

# Cross-compilation tools
CROSS_CC = aarch64-linux-gnu-gcc
AARCH64_CFLAGS = -include mock/spinlock.h -I./mock_aarch64 -I./mock -I../include -Wall -Wextra -g -fno-builtin -ffreestanding -nostdlib -Dmain=test_main -DLOG_LEVEL=3 -Dprintf=tfp_printf
AARCH64_ASFLAGS = -I../include -g
AARCH64_LDFLAGS = -T aarch64.ld -nostdlib

all: $(BUILD_DIR)/test_utils $(BUILD_DIR)/test_fifo

$(BUILD_DIR)/test_utils: $(BUILD_DIR)/test_utils.o $(BUILD_DIR)/utils.o
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/test_fifo: $(BUILD_DIR)/test_fifo.o $(BUILD_DIR)/fifo.o
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/utils.o: ../src/utils.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/fifo.o: ../src/fifo.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# AArch64 targets
AARCH64_COMMON_OBJS = $(AARCH64_BUILD_DIR)/aarch64_startup.o \
                      $(AARCH64_BUILD_DIR)/aarch64_env.o \
                      $(AARCH64_BUILD_DIR)/mini_uart.o \
                      $(AARCH64_BUILD_DIR)/printf.o \
                      $(AARCH64_BUILD_DIR)/utils_s.o \
                      $(AARCH64_BUILD_DIR)/utils_c.o \
                      $(AARCH64_BUILD_DIR)/fifo.o

$(AARCH64_BUILD_DIR)/%.o: %.c
	mkdir -p $(AARCH64_BUILD_DIR)
	$(CROSS_CC) $(AARCH64_CFLAGS) -c $< -o $@

$(AARCH64_BUILD_DIR)/%.o: %.S
	mkdir -p $(AARCH64_BUILD_DIR)
	$(CROSS_CC) $(AARCH64_ASFLAGS) -c $< -o $@

$(AARCH64_BUILD_DIR)/mini_uart.o: ../src/mini_uart.c
	mkdir -p $(AARCH64_BUILD_DIR)
	$(CROSS_CC) $(AARCH64_CFLAGS) -c $< -o $@

$(AARCH64_BUILD_DIR)/printf.o: ../src/printf.c
	mkdir -p $(AARCH64_BUILD_DIR)
	$(CROSS_CC) $(AARCH64_CFLAGS) -c $< -o $@

$(AARCH64_BUILD_DIR)/utils_s.o: ../src/utils.S
	mkdir -p $(AARCH64_BUILD_DIR)
	$(CROSS_CC) $(AARCH64_ASFLAGS) -c $< -o $@

$(AARCH64_BUILD_DIR)/utils_c.o: ../src/utils.c
	mkdir -p $(AARCH64_BUILD_DIR)
	$(CROSS_CC) $(AARCH64_CFLAGS) -c $< -o $@

$(AARCH64_BUILD_DIR)/fifo.o: ../src/fifo.c
	mkdir -p $(AARCH64_BUILD_DIR)
	$(CROSS_CC) $(AARCH64_CFLAGS) -c $< -o $@

$(AARCH64_BUILD_DIR)/test_fifo.img: $(AARCH64_COMMON_OBJS) $(AARCH64_BUILD_DIR)/test_fifo.o
	$(CROSS_CC) $(AARCH64_LDFLAGS) $^ -o $(AARCH64_BUILD_DIR)/test_fifo.elf
	aarch64-linux-gnu-objcopy -O binary $(AARCH64_BUILD_DIR)/test_fifo.elf $@

$(AARCH64_BUILD_DIR)/test_utils.img: $(AARCH64_COMMON_OBJS) $(AARCH64_BUILD_DIR)/test_utils.o
	$(CROSS_CC) $(AARCH64_LDFLAGS) $^ -o $(AARCH64_BUILD_DIR)/test_utils.elf
	aarch64-linux-gnu-objcopy -O binary $(AARCH64_BUILD_DIR)/test_utils.elf $@

test: all
	./$(BUILD_DIR)/test_utils
	./$(BUILD_DIR)/test_fifo

test-aarch64: $(AARCH64_BUILD_DIR)/test_fifo.img $(AARCH64_BUILD_DIR)/test_utils.img
	@echo "Running AArch64 tests in QEMU..."
	qemu-system-aarch64 -M raspi3b -kernel $(AARCH64_BUILD_DIR)/test_fifo.img -nographic -serial null -serial mon:stdio -semihosting
	qemu-system-aarch64 -M raspi3b -kernel $(AARCH64_BUILD_DIR)/test_utils.img -nographic -serial null -serial mon:stdio -semihosting
	@echo "All AArch64 tests passed!"

clean:
	rm -rf $(BUILD_DIR) $(AARCH64_BUILD_DIR)
