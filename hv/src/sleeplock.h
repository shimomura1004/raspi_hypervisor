#ifndef _SLEEPLOCK_H
#define _SLEEPLOCK_H

#include "spinlock.h"

struct sleeplock {
    unsigned int locked;
    struct spinlock lk;

    char *name;
    unsigned long vcpu_id;
};

void init_sleeplock(struct sleeplock *lock, char *name);
void acquire_sleeplock(struct sleeplock *lock);
void release_sleeplock(struct sleeplock *lock);

#endif
