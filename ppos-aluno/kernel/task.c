// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Gerência básica de tarefas.

void task_init()
{
    // Inicia a task kernel
    task_kernel.id = 0;
    task_kernel.name = "kernel";
    ctx_create(struct task_kernel.context, NULL,
               NULL, NULL, 0);
    task_kernel.status = Task_status.EXEC;
    task_kernel.parent = NULL;

    task_atual = &task_kernel;

    return;
}

struct task_t * task_create(char *name, void (*entry)(void *), void *arg) {
    
}

void task_term()
{
}

