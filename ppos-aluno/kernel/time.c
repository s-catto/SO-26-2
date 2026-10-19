// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Gerência básica do tempo.

// GRR20221224 Seijiro Yanagiya Midzuno
// GRR20232333 Sofia Barbosa Candiotto 

#include <stddef.h>
#include <stdio.h>
#include "hardware/cpu.h"
#include "lib/queue.h"
#include "lib/pplibc.h"
#include "dispatcher.h"
#include "time.h"
#include "task.h"

static unsigned int elapsed_time = 0;

extern struct queue_t *ready_queue;
extern struct queue_t *sleep_queue;

void time_tick(int sig)
{
    (void)sig;

    elapsed_time += TICK;

    //printk("oii\n");

    if (!task_atual)
        return;

    if (task_atual->quantum > 0)
        task_atual->quantum--;

    if (task_atual == &task_kernel)
        return;

    if (task_atual->status != EXEC)
        return;

    // Se o quantum acabou e a task é de usuário
    // reseta o quantum da task e a reinsere na fila de prontas.
    // A troca de contexto não deve ocorrer no próprio handler do timer.
    if (task_atual->quantum == 0 && task_atual->id > 0) {
        task_atual->status = READY;
        task_atual->quantum = QUANTUM;
        task_yield();
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

