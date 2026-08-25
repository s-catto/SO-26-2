// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Dispatcher: gerencia os estados das tarefas.

#include <stddef.h>

void user_main(void *arg);

void dispatcher_init()
{
}

int task_switch(struct task_t *task) {
    struct task_t* task_prox;

    if (task) {
        task_prox = task;
    } else {
        task_prox = task_atual->parent;
    }

    task_atual->status = SUSP;
    task_prox->status = EXEC;

    ctx_switch(task_atual->context, task_prox->context);

    task_atual = task_prox;

    return NOERROR;
}

void dispatcher_term()
{
}

void dispatcher()
{
    task_user = task_create("user", user_main, NULL);
    if (!task_user)
        return;
    task_switch(task_user);
    task_destroy(task_user);
}

