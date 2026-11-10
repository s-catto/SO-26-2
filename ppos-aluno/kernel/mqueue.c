// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Gerência de filas de mensagens

// GRR20221224 Seijiro Yanagiya Midzuno
// GRR20232333 Sofia Barbosa Candiotto

#include <stdlib.h>
#include <string.h>
#include "macros.h"
#include "mqueue.h"
#include "semaphore.h"
#include "lib/pplibc.h"
#include "lib/map.h"
#include "lib/queue.h"

#define MAX_QUEUES 64

typedef struct mqueue_t {
	struct queue_t* buffer;	// fila em si

	int	msg_size;		// tamanho máximo
	int msg_count;		// mensagens no buffer
	
	int s_buffer;		// semaforo da mqueue
	int s_vagas;		// controle de vagas
	int s_itens;		// avisa caso haja item

} mqueue_t;

struct map_t* mqueue_map = NULL;

void mqueue_init()
{
	mqueue_map = map_create(MAX_QUEUES);	
}

void mqueue_term()
{
	map_destroy(mqueue_map);
}


int mqueue_create(int max_msgs, int msg_size){
	struct mqueue_t* mqueue = malloc(sizeof(mqueue_t));
	if (!mqueue) 
        return ERROR;

	mqueue->buffer = queue_create();
	if (!(mqueue->buffer)) 
        return ERROR;

	mqueue->s_vagas = sem_create(max_msgs);
	mqueue->s_buffer = sem_create(1);
	mqueue->s_itens = sem_create(0);
    if ((mqueue->s_buffer == ERROR) || (mqueue->s_vagas == ERROR) || (mqueue->s_itens == ERROR))
        return ERROR;

	mqueue->msg_size = msg_size;
	mqueue->msg_count = 0;

	int id = map_put(mqueue_map, (void*) mqueue);
	if (id == ERROR) 
        return ERROR;

	return id;
}

int mqueue_destroy(int mqueue_id){
	struct mqueue_t* mqueue = map_get(mqueue_map, mqueue_id);
	if (!mqueue) 
        return ERROR;

	sem_destroy(mqueue->s_vagas);
	sem_destroy(mqueue->s_buffer);
	sem_destroy(mqueue->s_itens);

	while (queue_size(mqueue->buffer) > 0) {
		void *msg = queue_head(mqueue->buffer);

		queue_del(mqueue->buffer, msg);
		free(msg);
	}
    queue_destroy(mqueue->buffer);

	map_del(mqueue_map, mqueue_id);

	free(mqueue);

	return NOERROR;
}

int mqueue_send(int mqueue_id, void *msg){
    if (!mqueue_map || !msg)
        return ERROR;

    struct mqueue_t *mqueue = map_get(mqueue_map, mqueue_id);
    if (!mqueue)
        return ERROR;

	// checa se tem vaga
    sem_down(mqueue->s_vagas);
    // pede acesso ao buffer 
    sem_down(mqueue->s_buffer);


	// copia e envia a mensagem
    void *msg_cpy = malloc(mqueue->msg_size);
    if (!msg_cpy) {
        sem_up(mqueue->s_buffer);
        sem_up(mqueue->s_vagas);
        return ERROR;
    }
    memcpy(msg_cpy, msg, mqueue->msg_size);

    if (queue_add(mqueue->buffer, msg_cpy) < 0) {
        free(msg_cpy);
        sem_up(mqueue->s_buffer);
        sem_up(mqueue->s_vagas);
        return ERROR;
    }

    mqueue->msg_count++;

	// libera o buffer 
    sem_up(mqueue->s_buffer);
    // "avisa" que há itens disponíveis
    sem_up(mqueue->s_itens);

    return NOERROR;
}

int mqueue_recv(int mqueue_id, void *msg){
    int status;

    if (!mqueue_map || !msg)
        return ERROR;

    struct mqueue_t *mqueue = map_get(mqueue_map, mqueue_id);
    if (!mqueue)
        return ERROR;

	// espera haverem itens disponíveis
    status = sem_down(mqueue->s_itens);
    if (status != NOERROR)
        return ERROR;

    // pede acesso ao buffer
    status = sem_down(mqueue->s_buffer);
    if (status != NOERROR)
        return ERROR;

	// retira mensagem da fila
    void *msg_cpy = queue_head(mqueue->buffer);
    if (!msg_cpy) {
        sem_up(mqueue->s_buffer);
        sem_up(mqueue->s_itens);
        return ERROR;
    }

    memcpy(msg, msg_cpy, mqueue->msg_size);

    if (queue_del(mqueue->buffer, msg_cpy) == ERROR) {
        sem_up(mqueue->s_buffer);
        sem_up(mqueue->s_itens);
        return ERROR;
    }
    free(msg_cpy);

    mqueue->msg_count--;

    // libera o buffer
    sem_up(mqueue->s_buffer);
    // libera uma vaga
    sem_up(mqueue->s_vagas);

    return NOERROR;
}

// retorna o numero de mensagens em uma fila
// Retorno: número >= 0 ou ERROR
int mqueue_msgs(int mqueue_id){
    struct mqueue_t* mqueue = map_get(mqueue_map, mqueue_id);
	if (!mqueue) 
        return ERROR;

	sem_down(mqueue->s_buffer);

	int n = mqueue->msg_count;

	sem_up(mqueue->s_buffer);

	return n;
}