// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// Este arquivo PODE/DEVE ser alterado.

// Descritor de tarefas (TCB - Task Control Block).

#ifndef __PPOS_TCB__
#define __PPOS_TCB__

#include "ctx.h"

#define STACK_SIZE 16 * 1024

// Status possíveis da tarefa
typedef enum {
    READY,
    EXEC,
    SUSP,
    DONE,
} Task_status;

// Task Control Block (TCB), infos sobre uma tarefa
struct task_t
{
    int id;         // identificador da tarefa
    char *name;     // nome da tarefa
    struct ctx_t* context;  // contexto da tarefa
    Task_status status;     // pronta, executando, ...
    struct task_t* parent; // pai da task atual
    // ...             // demais informações, a completar
};

extern struct task_t task_kernel; 
extern struct task_t* task_atual; // ponteiro para a a task atual sendo executada

extern int id_i; // id da última task criada

#endif
