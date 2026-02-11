#include "sleeplock.h"
#include "cpu_core.h"
#include "debug.h"

void init_sleeplock(struct sleeplock *lock, char *name) {
    init_lock(&lock->lk, name);
    lock->locked = 0;
    lock->name = name;
}

void acquire_sleeplock(struct sleeplock *lock) {
    // スリープロックの状態を変更するので、まずスピンロックを取得する
    acquire_lock(&lock->lk);

    if (lock->locked) {
        PANIC("sleeplock %s is already held by vcpu %u", lock->name, lock->vcpu_id);
    }

    while (lock->locked) {
        // lock はユニークな ID として使われる
        // sleep 内部で lock->lk は解放される
        //   もし lock->lk が解放されない場合、他のプロセスが aquire_sleeplock を呼ぶと
        //   この関数の冒頭の aquire_lock でビジーループしてしまう
        //   さらに、全コアがこの状態になるとデッドロックになる
        sleep(lock, &lock->lk);
        // sleep を抜けるときには lock->lk は再取得されている
    }

    lock->locked = 1;
    lock->vcpu_id = current_pcpu()->current_vcpu->vcpu_id;
    
    release_lock(&lock->lk);
}

void release_sleeplock(struct sleeplock *lock) {
    acquire_lock(&lock->lk);

    if (!lock->locked || lock->vcpu_id != current_pcpu()->current_vcpu->vcpu_id) {
        PANIC("sleeplock not held by current vcpu");
    }

    lock->locked = 0;
    lock->vcpu_id = 0;

    wakeup(lock);

    release_lock(&lock->lk);
}
