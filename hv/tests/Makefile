CC = gcc
CFLAGS = -I./mock -I../include -Wall -Wextra -g -fno-builtin

.PHONY: all clean test

BUILD_DIR = build

all: $(BUILD_DIR)/test_utils $(BUILD_DIR)/test_fifo

$(BUILD_DIR)/test_utils: $(BUILD_DIR)/test_utils.o $(BUILD_DIR)/utils.o
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/test_fifo: $(BUILD_DIR)/test_fifo.o $(BUILD_DIR)/fifo.o
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/test_utils.o: test_utils.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test_fifo.o: test_fifo.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/utils.o: ../src/utils.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/fifo.o: ../src/fifo.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

test: all
	./$(BUILD_DIR)/test_utils
	./$(BUILD_DIR)/test_fifo

clean:
	rm -rf $(BUILD_DIR)
