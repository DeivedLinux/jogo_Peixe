#include "Shark.h"
#include <math.h>

#define getFish(Obj) ((Fish*)Obj->dados)


Shark* create_shark(int shark_value, SDL_Surface** shark_sprite){
	Shark* shark;
	shark = (Shark*)malloc(sizeof(Shark));

	shark->obj = create_object_random_pos(shark_value);
	shark->obj.life = settings->sharkLife; 
	shark->obj.sprite = shark_sprite;

	shark->state = SHARK_IDLE;
	shark->captureRange = settings->captureRange;
	shark->timeLapseToReproduction = settings->sharkTimeLapseToReproduction;
	shark->isLeader = 0;
	shark->reproducedThisRound = 0;
	shark->prey = NULL; 
	shark->boardValue = shark_value;

	return shark;
}

Shark* create_son_shark(Object father1, Object father2, int shark_value, int my_son_possib[8][2], SDL_Surface** shark_sprite){
	Shark* shark;
	shark = (Shark*)malloc(sizeof(Shark));

	shark->obj = create_object_father_pos(father1, father2, shark_value, my_son_possib);
	shark->obj.life = settings->sharkLife; 
	shark->obj.sprite = shark_sprite;

	shark->state = SHARK_IDLE;
	shark->captureRange = settings->captureRange;
	shark->timeLapseToReproduction = settings->sharkTimeLapseToReproduction;
	shark->isLeader = 0;
	shark->reproducedThisRound = 0;
	shark->prey = NULL; 
	shark->boardValue = shark_value;

	return shark;
}

void shark_update(Lista* li_shark, SDL_Surface** shark_shiny_sprite, int my_son_possib[8][2], int mov_possib[4][2], Lista* li_fish){
	
	Elem* li = *li_shark;
	Elem* ant = NULL; //Guardando o anterior para casos de remoção

	int count = 0;
	while(li != NULL){

		Shark* shark = getShark(li);

		if(isDead(shark->obj)){

			if(shark->isLeader && li->prox != NULL)
				give_leader_to_shark(getShark(li->prox), shark_shiny_sprite);

			li = removeElement(li_shark, li, ant);
		}
		else{ 
			if(has_shark_reproduced(shark)){
				//Já reproduziu nessa rodada - Não faz mais nada
			}
			else if(shark_reproduction(li, shark, my_son_possib, li_shark)){
				//Conseguiu reproduzir  - Não faz mais nada
			}
			else{
				shark_movement(shark, mov_possib, li_fish);		
			}
			shark->obj.life--;
				
			ant = li;
			li = li->prox;
		}
		count++;	
		shark->reproducedThisRound = 0;
	}
}

void shark_movement(Shark* shark, int mov_possib[4][2], Lista* li_fish){

		if(eat_fish_close(shark, li_fish)){
			printf("%i comi um peixe\n", shark->isLeader );
			return;
		}



		if(shark->isLeader){

			if(shark->prey == NULL)
				search_fish(shark, li_fish);
		}
		else{

		}


		
		if(shark->state == SHARK_CHASE && shark->isLeader){

			if(shark->prey == NULL)
				shark->state = SHARK_IDLE;
			else{

				Fish* fish = shark->prey;
				int nextX = shark->obj.x;
				int nextY = shark->obj.y;

				if(shark->obj.x < fish->obj.x){
					nextX += 1;
				}
				else if(shark->obj.x > fish->obj.x){
					nextX += -1;
				}
				else{
					if(shark->obj.y < fish->obj.y){
						nextY += 1;
					}
					else if(shark->obj.y > fish->obj.y){
						nextY += -1;
					}
				}

				if(!is_valid_pos(nextX, nextY)){
					adjuste_pos_circle_scenery(&nextX, &nextY);
				}

				shark->obj = move_object(shark->obj, nextX, nextY, shark->boardValue);
			}
		}


		if(shark->state == SHARK_IDLE){

			move_to_random_pos(&shark->obj, mov_possib, shark->boardValue);
		}
		else if(shark->state == SHARK_CHASE){

			/*if(shark->prey == NULL){
				shark->state = SHARK_IDLE;
			}
			else{

				Fish* fish = shark->prey;
				int nextX = shark->obj.x;
				int nextY = shark->obj.y;

				if(shark->obj.x < fish->obj.x){
					nextX += 1;
				}
				else if(shark->obj.x > fish->obj.x){
					nextX += -1;
				}
				else{
					if(shark->obj.y < fish->obj.y){
						nextY += 1;
					}
					else if(shark->obj.y > fish->obj.y){
						nextY += -1;
					}
				}

				//shark->obj = move_object(shark->obj, nextX, nextY, shark_value);
				Object oldObj = shark->obj;
				shark->obj = move_object(shark->obj, nextX, nextY, shark_value);
				create_trail(oldObj, shark->obj, nextX - shark->obj.x, nextY - shark->obj.y);

				int d = distance(shark->obj, fish->obj);
				if(d <= 1){
					shark->state = SHARK_IDLE;
					remove_lista_fish(li_fish, fish->obj);
					shark->prey = NULL;
				}
			}*/

		}

}

int shark_reproduction(Elem* li, Shark* shark, int my_son_possib[8][2], Lista* li_shark){

	if(canReproduce(shark->obj.life, settings->sharkLife, shark->timeLapseToReproduction)){

		Elem* aux = li->prox;
		while(aux != NULL){

			Shark* shark2 = getShark(aux);

			int d = distance(shark->obj, shark2->obj);
			if(isDistanceAcceptableToReproduce(d) && canReproduce(shark2->obj.life, settings->sharkLife, shark2->timeLapseToReproduction)){

				//shark->timeLapseToReproduction += 10;
				//shark2->timeLapseToReproduction += 10;
				Shark* son = create_son_shark(shark->obj, shark2->obj, shark->boardValue, my_son_possib, &shark->obj.sprite);
				shark2->reproducedThisRound = 1;
				if(son != NULL){
					insere_lista_final(li_shark, son);
				}

				return 1;
			}

			aux = aux->prox;
		}
	}

	return 0;
}

void give_leader_to_shark(Shark* shark, SDL_Surface** shark_shiny_sprite){
	
	shark->isLeader = 1;
	shark->obj.sprite = shark_shiny_sprite;
}

int has_shark_reproduced(Shark* shark){

	if(shark->reproducedThisRound == 1)
		return 1;

	return 0;
}

void search_fish(Shark* shark, Lista* li_fish){

	int betterDistance = 1000;

	Elem* li = *li_fish;
	while(li != NULL){

		Fish* fish = getFish(li);
		int d = distance(shark->obj, fish->obj);
		if(d <= shark->captureRange){

			if(d < betterDistance){
				shark->prey = fish;
				betterDistance = d;
				shark->state = SHARK_CHASE;
			}
		}
		

		li = li->prox;
	}

}

int eat_fish_close(Shark* shark, Lista* li_fish){

	Elem* li = *li_fish;
	Elem* ant = NULL;

	while(li != NULL){

		Fish* fish = getFish(li);
		int d = distance(shark->obj, fish->obj);
		if(d <= 2){

			if(d == 2 && abs(shark->obj.x - fish->obj.x) == 2 || abs(shark->obj.y - fish->obj.y) == 2){

			}
			else{
				shark->obj.life += 10;
				removeElement(li_fish, li, ant);
				return 1;
			}
		}
		
		ant = li;
		li = li->prox;
	}

	return 0;
}

