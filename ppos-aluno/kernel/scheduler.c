// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Escalonador de tarefas prontas.

#include "scheduler.h"

void sched_init()
{
}

void sched_term()
{
}

// função escalonador: devolve a próxima tarefa a escalonar na fila (FCFS)
struct task_t *scheduler(struct queue_t *ready_queue)
{
    if (ready_queue == NULL)
        return NULL;
    
    // Retorna a primeira tarefa da fila (FCFS - First Come First Served)
    return queue_head(ready_queue);
}

// muda a prioridade de uma tarefa (da tarefa atual se task == NULL)
void sched_setprio(struct task_t *task, int prio)
{
    // Será implementado em um projeto posterior
}

// obtém a prioridade de uma tarefa (da tarefa atual se task == NULL)
int sched_getprio(struct task_t *task)
{
    // Será implementado em um projeto posterior
    return 0;
}

