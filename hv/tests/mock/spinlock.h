#ifndef _SPINLOCK_MOCK_H
#define _SPINLOCK_MOCK_H

#ifndef _SPINLOCK_H
#define _SPINLOCK_H
#define _REAL_SPINLOCK_SKIPPED
#endif

struct spinlock {
    int dummy;
};

static inline void init_lock(struct spinlock *lock, char *name) {
    (void)lock;
    (void)name;
}

static inline void acquire_lock(struct spinlock *lock) {
    (void)lock;
}

static inline void release_lock(struct spinlock *lock) {
    (void)lock;
}

#endif
