#ifndef LISTA_H_
#define LISTA_H_

#include "Agent.h"

#define getShark(Obj) ((Shark)Obj->dados)

struct elemento{
	void* dados;
	struct elemento *prox;
};
typedef struct elemento Elem;

typedef struct elemento* Lista;



Lista* cria_lista();

void libera_lista(Lista* li);

int insere_lista_final(Lista* li,void* element);


#endif