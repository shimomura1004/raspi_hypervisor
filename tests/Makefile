CC = gcc
CFLAGS = -I./mock -I../hv/include -Wall -Wextra -g -fno-builtin

.PHONY: all clean test

all: test_utils test_fifo

test_utils: test_utils.o utils.o
	$(CC) $(CFLAGS) -o $@ $^

test_fifo: test_fifo.o fifo.o
	$(CC) $(CFLAGS) -o $@ $^

test_utils.o: test_utils.c
	$(CC) $(CFLAGS) -c $< -o $@

test_fifo.o: test_fifo.c
	$(CC) $(CFLAGS) -c $< -o $@

utils.o: ../hv/src/utils.c
	$(CC) $(CFLAGS) -c $< -o $@

fifo.o: ../hv/src/fifo.c
	$(CC) $(CFLAGS) -c $< -o $@

test: all
	./test_utils
	./test_fifo

clean:
	rm -f *.o test_utils test_fifo
