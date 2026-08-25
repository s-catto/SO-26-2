// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Dispatcher: gerencia os estados das tarefas.

void dispatcher_init()
{
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

