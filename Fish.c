#include "Fish.h"

#define getFish(Obj) ((Fish*)Obj->dados)
#define getShark(Obj) ((Shark*)Obj->dados)
#define getTrail(Obj) ((Trail*)Obj->dados)

Lista* li_shark;
Lista* m_li_trail;
Lista* li_fish;

SDL_Surface** fish_sprite[4];
SDL_Surface** fish_shiny_sprite[4];
SDL_Surface** m_trail_sprite;

int m_mov_possib[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };
int m_son_possib[8][2] = { {-1, -1}, {-1, 0}, {-1,  1}, {0,  1},
							{ 1,  1}, {1,  0}, { 1, -1}, {0, -1}};
int my_fish_value = 0;
int my_trail_value = 0;


void setupFishVariables(Lista* listaShark, Lista* listaFish, Lista* listaTrail, SDL_Surface* fishSprite[4], SDL_Surface* fishShinySprite[4],
					     SDL_Surface* trail_sprite, int movPossib[4][2], int  sonPossib[8][2], int fishValue, int trailValue){
	
	li_shark = listaShark;
	li_fish = listaFish;
	m_li_trail = listaTrail;

	
	my_fish_value = fishValue;
	my_trail_value = trailValue;

	for(int i = 0; i < 4; i++){
		fish_sprite[i] = fishSprite[i];
		fish_shiny_sprite[i] = fishShinySprite[i];
	}

	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 2; j ++){
			m_mov_possib[i][j] = movPossib[i][j];
		}
	}
	m_trail_sprite = trail_sprite;

	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 2; j ++){
			m_son_possib[i][j] = sonPossib[i][j];
		}
	}

	setupTrail(listaTrail, my_trail_value);
}

Fish* create_fish(){
	Fish* fish;
	fish = (Fish*)malloc(sizeof(Fish));

	fish->obj = create_object_random_pos(my_fish_value);
	fish->obj.life = settings->fishLife; 
	fish->obj.sprite = fish_sprite[0];

	fish->state = FISH_IDLE;
	fish->perceptionRange = settings->perceptionRange;
	fish->timeLapseToReproduction = settings->fishTimeLapseToReproduction;
	fish->isLeader = 0;
	fish->reproducedThisRound = 0;
	fish->predator = NULL; 
	fish->trail = NULL;
	fish->boardValue = my_fish_value;
	fish->orientation = 0;

	return fish;
}

Fish* create_son_fish(Object father1, Object father2){
	Fish* fish;
	fish = (Fish*)malloc(sizeof(Fish));

	fish->obj = create_object_father_pos(father1, father2, my_fish_value, m_son_possib);
	fish->obj.life = settings->fishLife; 
	fish->obj.sprite = fish_sprite[0];

	fish->state = FISH_IDLE;
	fish->perceptionRange = settings->perceptionRange;
	fish->timeLapseToReproduction = settings->fishTimeLapseToReproduction;
	fish->isLeader = 0;
	fish->reproducedThisRound = 0;
	fish->predator = NULL; 
	fish->trail = NULL;
	fish->boardValue = my_fish_value;
	fish->orientation = 0;

	return fish;
}

void fish_update(){

	Elem* li = (*li_fish);
	Elem* ant = NULL; //Guardando o anterior para casos de remoção
	
	while(li != NULL){

		Fish* fish = getFish(li);
		
		if(isDead(fish->obj)){

			if(fish->isLeader && li->prox != NULL)
				give_leader_to_fish(getFish(li->prox));

			li = removeElement(li_fish, li, ant);
		}
		else{ 
			if(has_fish_reproduced(fish)){
				//Já reproduziu nessa rodada - Não faz mais nada
				//printf("ja reproduziu\n");
			}
			else if(fish_reproduction(li, fish)){
				//Conseguiu reproduzir  - Não faz mais nada
				//printf("reproduziu agora!\n");
			}
			else{
				fish_movement(fish);		
			}
			fish->obj.life--;
				
			ant = li;
			li = li->prox;
		}
		fish->reproducedThisRound = 0;
	}
}


void fish_movement(Fish* fish) {

	/*Object sharkOldObj = shark->obj;
	if(eat_fish_close(shark)){

		if(shark->isLeader){
			create_shark_trail(sharkOldObj, shark->obj);
		}

		return;
	}*/


	if(fish->isLeader){

		if(fish->predator == NULL)
			search_predador(fish);// search_fish(shark);
	}
	else{

		/*if(has_trail_pos(shark->obj.x, shark->obj.y, m_shark_value)){
			
			Trail* trail = get_trail_by_pos(shark->obj.x, shark->obj.y);
			if(trail != NULL){

				int sorted = rand() % 100;
				if(sorted < trail->str){
					
					int* coord =  get_dir_coordinates(trail->dir);
					int posX = shark->obj.x + coord[0];
					int posY = shark->obj.y + coord[1];
					if(is_free_pos(posX, posY)){
						shark->obj = move_object(shark->obj, posX, posY, m_shark_value);
						return;
					}

				}

			}
		}
		else if(shark->trail == NULL){
			search_trail(shark);
		}*/
	}


	
	if(fish->state == FISH_RUNNING && fish->isLeader){

		if( fish->predator == NULL)
			fish->state = FISH_IDLE;
		else{
			int nextX = fish->obj.x;
			int nextY = fish->obj.y;

			int *right_coord = get_right_based_on_dir(fish->orientation);
			int *left_coord = get_left_based_on_dir(fish->orientation);

			nextX += right_coord[0];
			nextY += right_coord[1];

			printf("%i %i -- %i %i\n", fish->obj.x, fish->obj.y, nextX, nextY);

			if(!is_valid_pos(nextX, nextY)){
				adjuste_pos_circle_scenery(&nextX, &nextY);
			}

			if(is_free_pos(nextX, nextY)){
				
				Object oldObj = fish->obj;
				fish->obj = move_object(fish->obj, nextX, nextY, fish->boardValue);
				
				//create_shark_trail(sharkOldObj, shark->obj);
				if(distance(oldObj, fish->obj) == 0){
					fish->predator = NULL;
					fish->state = FISH_IDLE;
				}
				else{
					fish->orientation += 1;
				}
			}

			
		}
			/*if(shark->obj.x < fish->obj.x){
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
			}*/

			//if(!is_valid_pos(nextX, nextY)){
			//	adjuste_pos_circle_scenery(&nextX, &nextY);
			//}


			//criar trail aqui
			/*Object oldObj = shark->obj;
			shark->obj = move_object(shark->obj, nextX, nextY, shark->boardValue);
			create_shark_trail(sharkOldObj, shark->obj);
			if(distance(oldObj, shark->obj) == 0){
				shark->prey = NULL;
				shark->state = SHARK_IDLE;
			}*/
		
	}
	else if(fish->state == FISH_RUNNING){

		/*if( shark->trail == NULL)
			shark->state = SHARK_IDLE;
		else{
			Trail* trail = shark->trail;
			int nextX = shark->obj.x;
			int nextY = shark->obj.y;

			if(shark->obj.x < trail->obj.x){
				nextX += 1;
			}
			else if(shark->obj.x > trail->obj.x){
				nextX += -1;
			}
			else{
				if(shark->obj.y < trail->obj.y){
					nextY += 1;
				}
				else if(shark->obj.y > trail->obj.y){
					nextY += -1;
				}
			}

			if(!is_valid_pos(nextX, nextY)){
				adjuste_pos_circle_scenery(&nextX, &nextY);
			}

			shark->obj = move_object(shark->obj, nextX, nextY, shark->boardValue);
			if(distance(sharkOldObj, shark->obj) == 0){
				shark->trail = NULL;
				shark->state = SHARK_IDLE;
			}
		}*/
	}

	if(fish->state == FISH_IDLE){

		move_to_random_pos(&fish->obj, m_mov_possib, fish->boardValue);
	}
}

int* get_right_based_on_dir(int dir){

	int *coord = (int*)malloc(2 * sizeof(int));

	coord[0] = 0;
	coord[1] = -1;


	if(dir == 0){
		coord[0] = 1;
		coord[1] = 0;
	}
	if(dir == 1){
		coord[0] = 0;
		coord[1] = 1;
	}
	if(dir == 2){
		coord[0] = -1;
		coord[1] = 0;
	}
	if(dir == 3){
		coord[0] = 0;
		coord[1] = -1;
	}


	return coord;
}

int* get_left_based_on_dir(int dir){

	int *coord = (int*)malloc(2 * sizeof(int));

	coord[0] = 0;
	coord[1] = -1;


	if(dir == 0){
		coord[0] = -1;
		coord[1] = 0;
	}
	if(dir == 1){
		coord[0] = 0;
		coord[1] = -1;
	}
	if(dir == 2){
		coord[0] = 1;
		coord[1] = 0;
	}
	if(dir == 3){
		coord[0] = 0;
		coord[1] = 1;
	}


	return coord;
}

void give_leader_to_fish(Fish* fish){

	fish->isLeader = 1;
	fish->obj.sprite = fish_shiny_sprite[fish->orientation];
}

int has_fish_reproduced(Fish* fish){

	if(fish->reproducedThisRound == 1)
		return 1;

	return 0;
}

int fish_reproduction(Elem* li, Fish* fish){

	if(canReproduce(fish->obj.life, settings->fishLife, fish->timeLapseToReproduction)){

		Elem* aux = li->prox;
		while(aux != NULL){

			Fish* fish2 = getFish(aux);

			int d = distance(fish->obj, fish2->obj);
			if(isDistanceAcceptableToReproduce(d) && canReproduce(fish2->obj.life, settings->fishLife, fish2->timeLapseToReproduction)){

				//shark->timeLapseToReproduction += 10;
				//shark2->timeLapseToReproduction += 10;
				Fish* son = create_son_fish(fish->obj, fish2->obj);
				fish2->reproducedThisRound = 1;
				if(son != NULL){
					insere_lista_final(li_fish, son);
				}

				return 1;
			}

			aux = aux->prox;
		}
	}


	return 0;
}

void search_predador(Fish* fish){

	int betterDistance = 1000;

	Elem* li = *li_shark;
	while(li != NULL){

		Shark* shark = getShark(li);
		int d = distance(shark->obj, fish->obj);
		if(d <= fish->perceptionRange){

			if(d < betterDistance){
				fish->predator = shark;
				betterDistance = d;
				fish->state = FISH_RUNNING;
				printf("lider achou um predator proximo\n");
			}
		}
		
		li = li->prox;
	}
}