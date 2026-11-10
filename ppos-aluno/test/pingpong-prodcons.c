// PingPongOS - PingPong Operating System

// Produtores e consumidores

// GRR20221224 Seijiro Yanagiya Midzuno
// GRR20232333 Sofia Barbosa Candiotto

#include <stdlib.h>
#include "lib/queue.h"
#include "lib/pplibc.h"
#include "syscall.h"

#define N_PRODS 3
#define N_CONS  2
#define N_ITENS N_PRODS * N_CONS * 10

struct task_t *prod1, *prod2, *prod3; // produtores
struct task_t *cons1, *cons2;         // consumidores

struct queue_t *buffer;               

int s_vaga, s_buffer, s_item;   // semáforos necessários


void body_produtor(void *arg)
{
    for(int i = 0; i < N_ITENS / N_PRODS ; i++) {
        // cria item
        int *item = malloc(sizeof(int));
        *item = rand() % 100;

        // checa se tem vaga
        sem_down(s_vaga);
        // pede acesso ao buffer 
        sem_down(s_buffer);

        // coloca o item no buffer
        queue_add(buffer, item);

        printk("%s produziu %d\n", arg, *item);

        // libera o buffer 
        sem_up(s_buffer);
        // "avisa" que há itens disponíveis
        sem_up(s_item);

        task_sleep(1000);
    }

    task_exit(NOERROR);
}

void body_consumidor(void *arg)
{  
    for(int i = 0; i < N_ITENS / N_CONS; i++) {
        // espera haverem itens disponíveis
        sem_down(s_item);

        // pede acesso ao buffer
        sem_down(s_buffer);

        // consome item
        int *item = queue_head(buffer);
        printk("%s consumiu %d\n", arg, *item);
        queue_del(buffer, item);
        free(item);

        // libera o buffer
        sem_up(s_buffer);
        // libera uma vaga
        sem_up(s_vaga);

        task_sleep(1000);
    }

    task_exit(NOERROR);
}

void user_main(void *arg) 
{   
    int id;
    char *name;

    name = task_name(NULL);
    id   = task_id(NULL);
    printk("%5u ms: %s (id %d): inicio\n", time(), name, id);

    srand(time());

    // cria buffer
    buffer = queue_create();
    if (!buffer)
        task_exit(ERROR);

    // cria semaforos
    s_buffer = sem_create(1);
    s_vaga = sem_create(5);
    s_item = sem_create(0);
    if ((s_buffer == ERROR) || (s_vaga == ERROR) || (s_item == ERROR))
        task_exit(ERROR);

    // cria tarefas de produtores
    prod1 = task_create("p1", body_produtor, "P1");
    prod2 = task_create("p2", body_produtor, "P2");
    prod3 = task_create("p3", body_produtor, "P3");
    if (!prod1 || !prod2 || !prod3)
        task_exit(ERROR);

    // cria tarefas de consumidores
    cons1 = task_create("c1", body_consumidor, "\t\t\tC1");
    cons2 = task_create("c2", body_consumidor, "\t\t\tC2");
    if (!cons1 || !cons2)
        task_exit(ERROR);

    task_sleep(10000);

    // espera tarefas
    task_wait(prod1);
    task_wait(prod2);
    task_wait(prod3);
    task_wait(cons1);
    task_wait(cons2);

    // destroi tarefas
    task_destroy(prod1);
    task_destroy(prod2);
    task_destroy(prod3);
    task_destroy(cons1);
    task_destroy(cons2);

    // destroi semaforos
    sem_destroy(s_buffer);
    sem_destroy(s_item);
    sem_destroy(s_vaga);

    //destroi buffer
    int *item = queue_head(buffer);
    while(item) {
        free(item);
        item = queue_next(buffer);
    }
    queue_destroy(buffer);

    printk("%5u ms: %s fim\n", time(), name);
    task_exit(NOERROR);
}