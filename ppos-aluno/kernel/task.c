// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Gerência básica de tarefas.

// GRR20221224 Seijiro Yanagiya Midzuno
// GRR20232333 Sofia Barbosa Candiotto

#include "task.h"
#include "dispatcher.h"
#include "time.h"
#include "memory.h"
#include <stdlib.h>
#include "lib/pplibc.h"
#include "lib/queue.h"

struct task_t task_kernel;
struct task_t *task_atual;
int id_i;

extern struct queue_t *ready_queue;
struct queue_t *sleep_queue;

void task_init()
{
    // Inicia a task kernel
    task_kernel.id = 0;
    task_kernel.name = "kernel";
    task_kernel.status = EXEC;
    task_kernel.parent = NULL;

    task_kernel.birth_time = time();
    task_kernel.act_time = 0;
    task_kernel.exec_time = 0;           
    task_kernel.acts = 1;
    task_kernel.wait_queue = queue_create();
    task_kernel.wait_code = NOERROR;
    task_kernel.wake_time = 0;

    sleep_queue = queue_create();

    // Aponta task atual para task kernel
    task_atual = &task_kernel;

    // Inicializa o id da última task criada ('0' por ser a primeira)
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
    task->parent = &task_kernel;
    task->wait_queue = queue_create();
    if (task->wait_queue == NULL) {
        mem_free(stack);
        mem_free(task);
        return NULL;
    }
    task->wait_code = NOERROR;
    task->wake_time = 0;

    task->prioe = 0;
    task->priod = task->prioe;

    task->quantum = QUANTUM;
    task->birth_time = time();
    task->act_time = -1;
    task->exec_time = 0;           
    task->acts = 0; 
    
    // Adiciona a tarefa à fila de prontas
    extern void enqueue_task(struct task_t *task);
    enqueue_task(task);

    return task;
}

int task_destroy(struct task_t *task) {
    if (!task)
        return NOERROR;

    if (sleep_queue != NULL && queue_has(sleep_queue, task))
        queue_del(sleep_queue, task);

    if (task->wait_queue != NULL)
        queue_destroy(task->wait_queue);

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

int task_wait(struct task_t *task)
{
    if (task == NULL || task == task_atual)
        return ERROR;

    if (task->status == TERM)
        return task->exit_code;

    if (queue_has(task->wait_queue, task_atual))
        return task_atual->wait_code;

    if (queue_has(ready_queue, task_atual))
        queue_del(ready_queue, task_atual);

    task_atual->wait_code = NOERROR;
    task_atual->status = SUSP;
    queue_add(task->wait_queue, task_atual);
    task_switch(NULL);

    return task_atual->wait_code;
}

void task_sleep(int t)
{
    if (t <= 0 || task_atual == NULL || task_atual == &task_kernel)
        return;

    if (queue_has(ready_queue, task_atual))
        queue_del(ready_queue, task_atual);

    task_atual->status = SUSP;
    task_atual->wake_time = time() + (unsigned int)t;

    if (sleep_queue != NULL && !queue_has(sleep_queue, task_atual))
        queue_add(sleep_queue, task_atual);

    task_switch(NULL);
}

void task_term()
{
    if (task_kernel.wait_queue != NULL)
        queue_destroy(task_kernel.wait_queue);

    if (sleep_queue != NULL)
        queue_destroy(sleep_queue);
}