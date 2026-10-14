// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Gerência básica do tempo.

// GRR20221224 Seijiro Yanagiya Midzuno
// GRR20232333 Sofia Barbosa Candiotto 

#include <stddef.h>
#include "hardware/cpu.h"
#include "lib/queue.h"
#include "dispatcher.h"
#include "time.h"
#include "task.h"

static unsigned int elapsed_time = 0;

extern struct queue_t *ready_queue;
extern struct queue_t *sleep_queue;

void time_tick(int sig)
{
    (void)sig;

    if (task_atual == NULL || task_atual == &task_kernel)
        return;

    if (task_atual->status != EXEC)
        return;

    elapsed_time += TICK;

    if (sleep_queue != NULL) {
        struct task_t *task = queue_head(sleep_queue);
        while (task != NULL) {
            struct task_t *next = queue_next(sleep_queue);
            if (task->wake_time <= elapsed_time) {
                queue_del(sleep_queue, task);
                task_awake(task);
            }
            task = next;
        }
    }

    if (task_atual->quantum > 0)
        task_atual->quantum--;

    // Se o quantum acabou e a task é de usuário
    // reseta o quantum da task e a reinsere na fila de prontas.
    // A troca de contexto não deve ocorrer no próprio handler do timer.
    if (task_atual->quantum == 0 && task_atual->id > 0) {
        task_atual->status = READY;
        task_atual->quantum = QUANTUM;
        if (!queue_has(ready_queue, task_atual))
            queue_add(ready_queue, task_atual);
    }
}

void time_init()
{
    elapsed_time = 0;
    hw_irq_enable(1);
    hw_irq_handle(IRQ_TIMER, time_tick);
    hw_timer(TICK, TICK);
}

void time_term()
{
    hw_timer(0, 0);
    hw_irq_handle(IRQ_TIMER, NULL);
    hw_irq_enable(0);
}

unsigned int time()
{
    return elapsed_time;
}

