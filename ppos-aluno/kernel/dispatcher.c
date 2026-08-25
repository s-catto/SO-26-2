// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Dispatcher: gerencia os estados das tarefas.

#include <stddef.h>
#include "lib/pplibc.h"
#include "task.h"

extern void user_main(void *arg);

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

    printk("%s\n", task_prox->name);

    task_atual->status = SUSP;
    task_prox->status = EXEC;

    printk("%d", ctx_switch(&task_atual->context, &task_prox->context));

    /*if (ctx_switch(&task_atual->context, &task_prox->context) == ERROR) {
        printk("ops\n");
        return ERROR;
    }*/

    return NOERROR;
}

void dispatcher_term()
{
}

void dispatcher()
{
    struct task_t* task_user = task_create("user", user_main, NULL);
    if (!task_user)
        return;
    printk("oiiiii\n");
    task_switch(task_user);
    task_destroy(task_user);
}

