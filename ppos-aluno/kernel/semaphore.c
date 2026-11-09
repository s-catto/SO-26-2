// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Semáforos e spinlocks

// GRR20221224 Seijiro Yanagiya Midzuno
// GRR20232333 Sofia Barbosa Candiotto

#include <stdlib.h>
#include "ppos.h"
#include "lib/map.h"
#include "lib/queue.h"

#define N_SEM 64

struct semaphore_t
{
    int lock;              // spinlock
    int id;
    int counter;
    struct queue_t *queue; // fila de tarefas
};

struct map_t *sem_map;

// trava um spin-lock (busy wait)
void spin_lock(int *lock) {
    if (!lock)
        return;

    while (__sync_fetch_and_or(lock, 1));
}

// libera um spin-lock
void spin_unlock(int *lock) {
    if (!lock)
        return;

    (*lock) = 0;
}

void sem_init()
{
    sem_map = map_create(N_SEM);
}

void sem_term()
{
    map_destroy(sem_map);
}

int sem_create(int value) 
{
    struct semaphore_t *sem = mem_alloc(sizeof(struct semaphore_t));
    if (!sem)
        return ERROR;

    sem->lock = 0;
    sem->counter = value;
    sem->queue = queue_create();

    sem->id = map_put(sem_map, sem);

    return sem->id;
}

int sem_destroy(int sem_id) 
{
    struct semaphore_t *sem = map_del(sem_map, sem_id);

    if (!sem)
        return ERROR;

    // acorda todas as tarefas da fila
    struct task_t *task = queue_head(sem->queue);

    while(task) {
        task_awake(task);
        task = queue_next(sem->queue);
    }

    queue_destroy(sem->queue);

    free(sem);

    return NOERROR;
}

int sem_down(int sem_id)
{
    struct semaphore_t *sem = map_get(sem_map, sem_id);
    if (!sem)
        return ERROR;

    spin_lock(&(sem->lock));

    sem->counter--;

    if (sem->counter < 0) {
        spin_unlock(&(sem->lock));
        task_suspend(sem->queue);
    } else {
        spin_unlock(&(sem->lock));
    }

    return NOERROR;
}

int sem_up(int sem_id) 
{
    struct semaphore_t *sem = map_get(sem_map, sem_id);
    if (!sem)
        return ERROR;

    spin_lock(&(sem->lock));

    sem->counter++;

    if (sem->counter <= 0) {
        struct task_t *task = queue_head(sem->queue);
        queue_del(sem->queue, task);
        task_awake(task);
    }

    spin_unlock(&(sem->lock));

    return NOERROR;
}