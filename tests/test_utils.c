#include <assert.h>
#include <stdio.h>
#include "utils.h"

int main() {
    printf("Running utils tests...\n");

    // Test abs
    assert(abs(-5) == 5);
    assert(abs(5) == 5);
    assert(abs(0) == 0);

    // Test strlen
    assert(strlen("hello") == 5);
    assert(strlen("") == 0);

    // Test strcmp
    assert(strcmp("abc", "abc") == 0);
    assert(strcmp("abc", "abd") < 0);
    assert(strcmp("abd", "abc") > 0);

    printf("All utils tests passed!\n");
    return 0;
}
