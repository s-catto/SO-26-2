// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Dispatcher: gerencia os estados das tarefas.

// GRR20221224 Seijiro Yanagiya Midzuno
// GRR20232333 Sofia Barbosa Candiotto 

#include <stddef.h>
#include "lib/pplibc.h"
#include "lib/queue.h"
#include "task.h"
#include "scheduler.h"
#include "time.h"

extern void user_main(void *arg);

// Fila de tarefas prontas
struct queue_t *ready_queue;

// Contador de tarefas de usuário
static int user_tasks = 0;

// Adiciona uma tarefa à fila de prontas (função interna)
void enqueue_task(struct task_t *task)
{
    if (task && ready_queue) {
        queue_add(ready_queue, task);
        // Incrementa o contador de tarefas de usuário
        user_tasks++;
    }
}

void dispatcher_init()
{
    // Cria a fila de tarefas prontas
    ready_queue = queue_create();
}

int task_switch(struct task_t *task) {

    struct task_t *task_prox;
    struct task_t *task_anterior;

    if (task_atual == NULL)
        return ERROR;

    // se for NULL, muda para a task mãe
    if (task) {
        task_prox = task;
    } else {
        task_prox = task_atual->parent;
    }

    if (!task_prox)
        return ERROR;
 
    task_anterior = task_atual;
    task_atual = task_prox;

    // Calcula e incrementa o tempo de execução da task anterior
    task_anterior->exec_time += (time() - task_anterior->act_time);
    task_anterior->act_time = -1; // Invalida o tempo da última ativação
    
    // Guarda o momento de ativação da task atual
    task_atual->act_time = time();
    task_atual->acts++; // Incrementa o número de ativações

    if (ctx_switch(&task_anterior->context, &task_prox->context) == ERROR)
        return ERROR;


    return NOERROR;
}

void task_run(struct task_t *task)
{
    if (!task || !ready_queue)
        return;
    
    // Retira a tarefa da fila de prontas
    queue_del(ready_queue, task);
    
    // Muda o status para EXEC
    task->status = EXEC;
    task->quantum = QUANTUM;

    // Transfere a CPU para ela
    task_switch(task);
}

void task_yield()
{
    if (!ready_queue)
        return;
    
    struct task_t *current = task_atual;
    
    // Muda o estado para READY
    current->status = READY;
    
    // Coloca no fim da fila de prontas
    queue_add(ready_queue, current);
    
    // Retorna ao kernel
    task_switch(NULL);
}

void task_suspend(struct queue_t *queue)
{
    if (!ready_queue)
        return;
    
    struct task_t *current = task_atual;
    
    // Ajusta o status para SUSPENSA
    current->status = SUSP;
    
    // Insere na fila (se não for nula)
    if (queue != NULL) {
        queue_add(queue, current);
    }
    
    // Retorna ao kernel
    task_switch(NULL);
}

void task_awake(struct task_t *task)
{
    if (!task || !ready_queue)
        return;
    
    // Se a tarefa estiver suspensa em alguma fila, retira-a
    // (percorre a fila da tarefa e remove se encontrar)
    // Para isso, seria necessário saber em qual fila ela está
    // Como não temos essa informação armazenada, vamos verificar se não está pronta
    if (task->status == SUSP) {
        // Ajusta o status para PRONTA
        task->status = READY;
        
        // Insere na fila de prontas
        queue_add(ready_queue, task);
    }
}

void task_exit(int exit_code)
{
    if (!ready_queue)
        return;
    
    struct task_t *current = task_atual;
    
    // Muda o estado para TERMINADA
    current->status = TERM;
    current->exit_code = exit_code;
    
    // Decrementa o contador de tarefas de usuário
    user_tasks--;
    
    // Retorna ao kernel
    task_switch(NULL);
}

void print_time (struct task_t* task) {
    // Se a task é nula, printa as informações da task atual
    if (!task)
        task = task_atual;
    
    printk("PPOS: task %3d (%s) %6d ms run, %6d ms cpu, %5d acts, exit code %3d\n", 
        task->id, task->name, time() - task->birth_time, task->exec_time, task->acts, task->exit_code);
    
    return;
}

void dispatcher_term()
{
    if (ready_queue)
        queue_destroy(ready_queue);
}

void dispatcher()
{
    // Cria a tarefa inicial de usuário (task_create a adiciona à fila e incrementa user_tasks)
    struct task_t *task_user = task_create("user_main", user_main, NULL);
    if (!task_user)
        return;
    
    // Enquanto houver tarefas de usuário
    while (user_tasks > 0) {
        // Escolhe a próxima tarefa a executar
        struct task_t *next = scheduler(ready_queue);
        
        // Se o escalonador escolheu uma tarefa
        if (next != NULL) {
            // Trata a tarefa antes de executar
            if (next->status == READY) {
                // Executa a tarefa
                task_run(next);
                
                // Ao voltar ao dispatcher, trata a tarefa de acordo com seu estado
                switch (next->status) {
                    case READY:
                        // A tarefa continua pronta, já está na fila
                        break;
                    case TERM:
                        // A tarefa terminou, destrói-a
                        print_time(next);
                        task_destroy(next);
                        break;
                    case SUSP:
                        // A tarefa foi suspensa, já está em sua fila
                        break;
                    default:
                        break;
                }
            }
        }
    }

    print_time(NULL);
}

