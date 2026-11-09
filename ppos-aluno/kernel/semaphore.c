// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Semáforos e spinlocks

#include <stdatomic.h>

struct semaphore
{
    int lock;              // spinlock
    int id;
    int counter;
    struct queue_t *queue; // fila de tarefas
};

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

int sem_create(int value) 
{
    
}

int sem_destroy(int sem_id) 
{

}

int sem_down(int sem_id)
{

}

int sem_up(int sem_id) 
{

}