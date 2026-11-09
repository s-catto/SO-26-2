// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// Este arquivo PODE/DEVE ser alterado.

// Implementação do TAD Mapa de objetos

// GRR20221224 Seijiro Yanagiya Midzuno
// GRR20232333 Sofia Barbosa Candiotto

#include <stdio.h>
#include <stdlib.h>

struct map_t {
    int size;       // tamanho do mapa 
    int aloc;       // total alocado
    int last;       // último uid alocado

    void** objects;  // vetor de ponteiros
};

struct map_t *map_create(int size) {
    if (size < 0)
        return NULL;

    struct map_t *map = malloc(sizeof(struct map_t));
    if (!map)
        return NULL;

    map->objects = calloc(size, sizeof(void*));
    if (!map->objects) {
        free(map);
        return NULL;
    }

    map->size = size;
    map->aloc = 0;
    map->last = -1;
    
    return map;
}

int map_destroy(struct map_t *map) {
    free(map->objects);
    free(map);

    return 0;
}

int map_put(struct map_t *map, void *object) {
    if (!map || !object)
        return -1;

    // se o mapa está cheio
    if (map->aloc >= map->size)
        return -1;

    // se tem espaço no final, aloca no final
    if ((map->last + 1) < map->size) {
        map->objects[map->last + 1] = object;
        map->aloc++;
        return ++map->last;
    }

    // se não, procura espaço vago
    int i = 0;
    while (map->objects[i] != NULL)
        i++;

    map->objects[i] = object;
    map->aloc++;

    return i;
}

void *map_get(struct map_t *map, int id) {
    if (!map)
        return NULL;

    // caso id inválida
    if ((id >= map->size) || (id < 0))
        return NULL;

    return map->objects[id];
}

void *map_del(struct map_t *map, int id) {
    if (!map)
        return NULL;

    // caso id inválida
    if ((id >= map->size) || (id < 0))
        return NULL;

    void* object;

    // não permite "retirar" objetos inexistentes
    if (map->objects[id] != NULL) {
        map->aloc--;

        // reposisciona last, se for o caso
        if (id == map->last) {
            int i = map->last;
            // procura ultimo uid alocado
            while ((map->objects[i] == NULL) && (i >= 0))
                i--;

            map->last = i;
        }
    }

    object = map->objects[id];
    map->objects[id] = NULL;

    return object;
}

int map_items(struct map_t *map) {
    if (!map)
        return -1;

    return map->aloc;
}

int map_size(struct map_t *map) {
    if (!map)
        return -1;

    return map->size;
}

void map_print(char *name, struct map_t *map) {
    if (!map) {
        printf("%s: undef\n", name);
    }

    printf("%s: [ ", name);

    for (int i = 0; i < map->size; i++) {
        if (map->objects[i]) {
            printf("* ");
        } else {
            printf("- ");
        }
    }

    printf("] (%d/%d)\n", map->aloc, map->size);

    return;
}
