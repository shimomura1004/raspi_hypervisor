#ifndef _SPINLOCK_H
#define _SPINLOCK_H

struct spinlock {
    unsigned long locked;
    char *name;
    long cpuid;
};

void init_lock(struct spinlock *lock, char *name);
void acquire_lock(struct spinlock *lock);
void release_lock(struct spinlock *lock);

#endif
