#include "Agent.h"
#include "Lista.h"

Lista* cria_lista(){
	Lista* li = (Lista*)malloc(sizeof(Lista));
	if(li != NULL)
		*li = NULL;

	return li;
}

void libera_lista(Lista* li){
	if(li != NULL){
		Elem* no;
		while((*li) != NULL){
			no = *li;
			*li = (*li)->prox;
			free(no);
		}
		free(li);
	}
}

int insere_lista_final(Lista* li,void* element)
{
	if(li == NULL) return 0;

	Elem* no = (Elem*) malloc(sizeof(Elem));
	if(no == NULL) return 0;
	no->dados = element;
	no->prox = NULL;

	if((*li) == NULL){ //Lista vazia
		*li = no;
	}
	else{
		Elem* aux = *li;
		while(aux->prox != NULL){
			aux = aux->prox;
		}
		aux->prox = no;
	}

	return 1;
}


int remove_lista_fish(Lista* li, Object obj){

	Elem* aux = *li;
	Elem* ant = NULL;
	while(aux != NULL){

		Fish* fish = getFish(aux);
		if(fish->obj.x == obj.x && fish->obj.y == obj.y){
			if(ant == NULL){
				*li = aux->prox;
				free(aux);
				return 1;
			}
			else{
				ant->prox = aux->prox;
				free(aux);
				return 1;
			}
		}

		ant = aux;
		aux = aux->prox;
	}

	return 1;
}