// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Gerência básica de tarefas.

#include "task.h"
#include "memory.h"
#include <stdlib.h>
#include "lib/pplibc.h"

struct task_t task_kernel;
struct task_t *task_atual;
int id_i;

void task_init()
{
    // Inicia a task kernel
    task_kernel.id = 0;
    task_kernel.name = "kernel";
    task_kernel.status = EXEC;
    task_kernel.parent = NULL;

    // Aponta task atual para task kernel
    task_atual = &task_kernel;

    id_i = 0;

    return;
}

struct task_t * task_create(char *name, void (*entry)(void *), void *arg) {
    
    // Aloca a task
    struct task_t* task = mem_alloc(sizeof(struct task_t));
    if (!task)
        return NULL;  
    
    // Aloca a stack
    void* stack = mem_alloc(STACK_SIZE);
    if (!stack) {
        mem_free(task);
        return NULL;
    }

    // Preenche a task
    task->name = name;
    task->id = ++id_i;   

    if( ctx_create(&(task->context), entry, arg, stack, STACK_SIZE) == ERROR) {
        mem_free(stack);
        mem_free(task);
        return NULL;
    } 
    
    task->status = READY;
    task->parent = task_atual;

    return task;
}

int task_destroy(struct task_t *task) {
    if (!task)
        return NOERROR;

    // Free na stack e na struct
    mem_free(task->context.stack);
    mem_free(task);

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

void task_term()
{
}

