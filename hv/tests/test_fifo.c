#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "fifo.h"

// todo: テストコードに直接埋め込まず、モックにする
//       ホスト環境だけでなく、qemu-system-aarch64 を使った実機環境でのテストも追加する
unsigned long allocate_page() {
    // In our mock environment, just use malloc to grab 4096 bytes 
    // to simulate a hypervisor page allocation.
    return (unsigned long)malloc(4096);
}

void free_page(void *p) {
    free(p);
}

int main() {
    printf("Running fifo tests (testing hv logic with mocks)...\n");

    struct fifo *f = create_fifo();
    assert(f != NULL);

    // Initial state
    assert(is_empty_fifo(f) == 1);
    assert(is_full_fifo(f) == 0);
    assert(used_of_fifo(f) == 0);

    // Enqueue
    assert(enqueue_fifo(f, 100) == 0);
    assert(enqueue_fifo(f, 200) == 0);
    assert(enqueue_fifo(f, 300) == 0);

    assert(is_empty_fifo(f) == 0);
    assert(used_of_fifo(f) == 3);

    // Dequeue
    unsigned long val = 0;
    assert(dequeue_fifo(f, &val) == 0);
    assert(val == 100);
    assert(used_of_fifo(f) == 2);

    assert(dequeue_fifo(f, &val) == 0);
    assert(val == 200);
    assert(used_of_fifo(f) == 1);

    // Clear
    clear_fifo(f);
    assert(is_empty_fifo(f) == 1);
    assert(is_full_fifo(f) == 0);
    assert(used_of_fifo(f) == 0);

    // Fill the fifo
    int FIFO_SIZE = 256;
    for (int i = 0; i < FIFO_SIZE; i++) {
        assert(enqueue_fifo(f, i) == 0);
    }
    
    assert(is_full_fifo(f) == 1);
    assert(used_of_fifo(f) == FIFO_SIZE);

    // Enqueueing to a full queue should fail (-1)
    assert(enqueue_fifo(f, 999) == -1);

    destroy_fifo(f);

    printf("All fifo tests passed!\n");
    return 0;
}
