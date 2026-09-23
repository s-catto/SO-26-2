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

void time_tick(int sig)
{
    (void)sig;

    if (task_atual == NULL || task_atual == &task_kernel)
        return;

    if (task_atual->status != EXEC)
        return;

    elapsed_time += TICK;

    if (task_atual->quantum > 0)
        task_atual->quantum--;

    // Se o quantum acabou e a task é de usuário
    // reseta o quantum da task e a reinsere na fila de prontas 
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

