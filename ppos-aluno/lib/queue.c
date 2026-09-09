// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// Este arquivo PODE/DEVE ser alterado.

// Implementação do TAD fila genérica

// GRR20221224 Seijiro Yanagiya Midzuno
// GRR20232333 Sofia Barbosa Candiotto 

#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

// Estrutura de um nó da fila
typedef struct queue_node {
    void *item;
    struct queue_node *next;
} queue_node_t;

// Estrutura da fila
struct queue_t {
    queue_node_t *head;      // primeiro nó
    queue_node_t *tail;      // último nó
    queue_node_t *current;   // iterador
    int size;                // número de elementos
};

// Cria uma fila inicialmente vazia
struct queue_t *queue_create() {
    struct queue_t *q = (struct queue_t *)malloc(sizeof(struct queue_t));
    if (q == NULL)
        return NULL;
    
    q->head = NULL;
    q->tail = NULL;
    q->current = NULL;
    q->size = 0;
    
    return q;
}

// Destroi uma fila, liberando a memória alocada por ela
int queue_destroy(struct queue_t *queue) {
    if (queue == NULL)
        return ERROR;
    
    // Libera todos os nós
    queue_node_t *node = queue->head;
    while (node != NULL) {
        queue_node_t *next = node->next;
        free(node);
        node = next;
    }
    
    // Libera a estrutura da fila
    free(queue);
    
    return NOERROR;
}

// Adiciona um item no fim da fila
int queue_add(struct queue_t *queue, void *item) {
    if (queue == NULL || item == NULL)
        return ERROR;
    
    // Cria um novo nó
    queue_node_t *node = (queue_node_t *)malloc(sizeof(queue_node_t));
    if (node == NULL)
        return ERROR;
    
    node->item = item;
    node->next = NULL;
    
    // Se a fila está vazia
    if (queue->head == NULL) {
        queue->head = node;
        queue->tail = node;
        queue->current = node;  // iterador aponta para o primeiro
    } else {
        // Adiciona no fim
        queue->tail->next = node;
        queue->tail = node;
    }
    
    queue->size++;
    
    return NOERROR;
}

// Retira da fila o item com o valor indicado
int queue_del(struct queue_t *queue, void *item) {
    if (queue == NULL || item == NULL)
        return ERROR;
    
    // Procura o nó com o item
    queue_node_t *prev = NULL;
    queue_node_t *node = queue->head;
    
    while (node != NULL) {
        if (node->item == item) {
            // Encontrou o item
            
            // Se o item está sob o iterador, avança o iterador
            if (queue->current == node) {
                queue->current = node->next;
            }
            
            // Remove o nó da fila
            if (prev == NULL) {
                // Remove o primeiro nó
                queue->head = node->next;
            } else {
                prev->next = node->next;
            }
            
            // Se era o último nó
            if (queue->tail == node) {
                queue->tail = prev;
            }
            
            free(node);
            queue->size--;
            
            return NOERROR;
        }
        
        prev = node;
        node = node->next;
    }
    
    // Item não encontrado
    return ERROR;
}

// Informa se o item indicado está na fila
bool queue_has(struct queue_t *queue, void *item) {
    if (queue == NULL || item == NULL)
        return false;
    
    queue_node_t *node = queue->head;
    
    while (node != NULL) {
        if (node->item == item)
            return true;
        node = node->next;
    }
    
    return false;
}

// Informa o número de itens na fila
int queue_size(struct queue_t *queue) {
    if (queue == NULL)
        return ERROR;
    
    return queue->size;
}

// Põe o iterador no início da fila
void *queue_head(struct queue_t *queue) {
    if (queue == NULL)
        return NULL;
    
    queue->current = queue->head;
    
    if (queue->current == NULL)
        return NULL;
    
    return queue->current->item;
}

// Avança o iterador ao próximo item na fila
void *queue_next(struct queue_t *queue) {
    if (queue == NULL || queue->current == NULL)
        return NULL;
    
    queue->current = queue->current->next;
    
    if (queue->current == NULL)
        return NULL;
    
    return queue->current->item;
}

// Informa o item atualmente sob o iterador na fila
void *queue_item(struct queue_t *queue) {
    if (queue == NULL || queue->current == NULL)
        return NULL;
    
    return queue->current->item;
}

// Imprime os elementos de uma fila
void queue_print(char *name, struct queue_t *queue, void(func)(void *)) {
    if (queue == NULL) {
        printf("%s: undef\n", name);
        return;
    }
    
    printf("%s: [ ", name);
    
    queue_node_t *node = queue->head;
    while (node != NULL) {
        if (func != NULL) {
            func(node->item);
            printf(" ");
        } else {
            printf("undef ");
        }
        node = node->next;
    }
    
    printf("] (%d items)\n", queue->size);
}