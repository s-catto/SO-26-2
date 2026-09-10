// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Escalonador de tarefas prontas.

// GRR20221224 Seijiro Yanagiya Midzuno
// GRR20232333 Sofia Barbosa Candiotto 

#include "tcb.h"
#include "scheduler.h"

extern struct queue_t *ready_queue;

void sched_init()
{
    return;
}

void sched_term()
{
    return;
}

// função escalonador: devolve a tarefa com maior prioridade da fila e envelhece as demais (PRIOd)
struct task_t *scheduler(struct queue_t *ready_queue)
{
    if (ready_queue == NULL)
        return NULL;
    
    struct task_t *chosen = queue_head(ready_queue);
    struct task_t *task = queue_next(ready_queue);

    // escolhe a task mais prioritária
    while (task != NULL) {
        if (task->priod < chosen->priod)
            chosen = task;
        task = queue_next(ready_queue);
    }

    // retira da fila a task escolhida e reseta a prioridade
    queue_del(ready_queue, chosen);
    chosen->priod = sched_getprio(chosen);

    // envelhece as outras tasks
    task = queue_head(ready_queue);
    while (task != NULL) {
        if (task->priod > - 0)
            task->priod = task->priod - 1;

        task = queue_next(ready_queue);
    }

    return chosen;
}

// muda a prioridade de uma tarefa (da tarefa atual se task == NULL)
void sched_setprio(struct task_t *task, int prio)
{
    if (task == NULL) {
        task_atual->prioe = prio;
        return;
    }
    
    task->prioe = prio;

    return;
}

// obtém a prioridade de uma tarefa (da tarefa atual se task == NULL)
int sched_getprio(struct task_t *task)
{
    if (task == NULL)
        return task_atual->prioe;
    
    return task->prioe;
}

