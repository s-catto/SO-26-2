// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// Este arquivo PODE/DEVE ser alterado.

// Descritor de tarefas (TCB - Task Control Block).

#ifndef __PPOS_TCB__
#define __PPOS_TCB__

// Status possíveis da tarefa
typedef enum {
    READY = 0,
    EXEC = 1,
    DONE = 2,
}Task_status;

task_t task_kernel;
task_t* task_atual; 

// Task Control Block (TCB), infos sobre uma tarefa
struct task_t
{
    int id;         // identificador da tarefa
    char *name;     // nome da tarefa
    ctx_t context;  // contexto da tarefa
    int status;     // pronta, executando, ...
    task_t* parent; // pai da task atual
    // ...             // demais informações, a completar
};

#endif
