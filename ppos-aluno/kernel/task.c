// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Gerência básica de tarefas.

#include "task.h"
#include <stdlib.h>

void task_init()
{
    struct task_t task_kernel;
    // Inicia a task kernel
    task_kernel.id = 0;
    task_kernel.name = "kernel";
    task_kernel.status = EXEC;
    task_kernel.parent = NULL;

    // Aponta task atual para task kernel
    struct task_t *task_atual = &task_kernel;

    int id_i = 0;

    return;
}

struct task_t * task_create(char *name, void (*entry)(void *), void *arg) {
    
    // Aloca a task
    struct task_t* task = malloc(sizeof(struct task_t));
    if (!task)
        return NULL;

    // Aloca a stack
    void* stack = aligned_alloc(16, sizeof(STACK_SIZE));
    if (!stack)
        return NULL;

    // Preenche a task
    task->name = name;
    task->id = ++id_i;
    ctx_create(task->context, entry, arg, stack, STACK_SIZE);;
    task->status = READY;
    task->parent = task_atual;

    return task;
}

int task_destroy(struct task_t *task) {
    if (!task)
        return NOERROR;

    // Free na stack e na struct
    free(task->context->stack);
    free(task);

    return NOERROR;
}

int task_id(struct task_t *task) {
    if (task) 
        return task->id;

    return task_atual->id;
}

char *task_name(struct task_t *task) {
    if (task) 
        return task->name;

    return task_atual->name;
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

void task_term()
{
}

