// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Dispatcher: gerencia os estados das tarefas.

// GRR20221224 Seijiro Yanagiya Midzuno
// GRR20232333 Sofia Barbosa Candiotto 

#include <stddef.h>
#include "lib/pplibc.h"
#include "task.h"

extern void user_main(void *arg);

void dispatcher_init()
{
}

int task_switch(struct task_t *task) {
    struct task_t *task_prox;
    struct task_t *task_anterior;

    // se for NULL, muda para a task mãe
    if (task) {
        task_prox = task;
    } else {
        task_prox = task_atual->parent;
    }

    if (!task_prox)
        return ERROR;
 
    task_anterior = task_atual;

    // muda os status das tasks
    task_anterior->status = SUSP;
    task_prox->status = EXEC;

    task_atual = task_prox;

    if (ctx_switch(&task_anterior->context, &task_prox->context) == ERROR)
        return ERROR;

    return NOERROR;
}
void dispatcher_term()
{
}

void dispatcher()
{
    struct task_t* task_user = task_create("user_main", user_main, NULL);
    if (!task_user)
        return;
    task_switch(task_user);
    task_destroy(task_user);
}

