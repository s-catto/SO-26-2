// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Semáforos e spinlocks

#include <stdatomic.h>

// trava um spin-lock (busy wait)
void spin_lock(int *lock) {
    int key = 1;
    while (key)
        key = atomic_exchange(lock, key);
}

// libera um spin-lock
void spin_unlock(int *lock) {
    (*lock) = 0;
}

void sem_init()
{
}

void sem_term()
{
}

