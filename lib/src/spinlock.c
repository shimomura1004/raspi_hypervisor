#include "spinlock.h"

void init_lock(struct spinlock *lock, char *name) {
    lock->locked = 0;
    lock->name = name;
    lock->cpuid = -1;
}
