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

	//setupTrail(listaTrail, my_trail_value);
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
	fish->orientation = 40;
	fish->goingRight = 1;

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
	fish->orientation = 40;
	fish->goingRight = 1;

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

	Object fishOldObj = fish->obj;


	if(fish->isLeader){

		if(fish->predator == NULL)
			search_predador(fish);// search_fish(shark);
	}
	else{

		/*if(has_trail_pos(fish->obj.x, fish->obj.y, my_fish_value)){
			
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
			
		}*/
		search_trail_to_follow(fish);
	}


	
	if(fish->state == FISH_RUNNING && fish->isLeader){

		if( fish->predator == NULL)
			fish->state = FISH_IDLE;
		else{
			int nextX = fish->obj.x;
			int nextY = fish->obj.y;

			int *right_coord = get_right_based_on_dir(fish->orientation);
			int *left_coord = get_left_based_on_dir(fish->orientation);

			if(fish->goingRight){
				nextX += right_coord[0];
				nextY += right_coord[1];
			}
			else{
				nextX += left_coord[0];
				nextY += left_coord[1];
			}

			int tentarEsquerda = 0;

			//TENTANDO DIREITA
			if(!is_valid_pos(nextX, nextY)){
				adjuste_pos_circle_scenery(&nextX, &nextY);
			}

			if(is_free_pos(nextX, nextY) && board->spaces[nextX][nextY] != my_trail_value){

				Object oldObj = fish->obj;
				fish->obj = move_object(fish->obj, nextX, nextY, fish->boardValue);
				
				//create_shark_trail(sharkOldObj, shark->obj);
				if(distance(oldObj, fish->obj) == 0){
					tentarEsquerda = 1;
					if(fish->goingRight == 1)
						fish->goingRight = 0;
					else
						fish->goingRight = 1;

					//fish->predator = NULL;
					//fish->state = FISH_IDLE;
				}
				else{
					fish->orientation += 1;
				}
			}
			else{
				printf("nao foi uma posicao valida\n");
				tentarEsquerda = 1;
				if(fish->goingRight == 1)
					fish->goingRight = 0;
				else
					fish->goingRight = 1;
			}

			if(tentarEsquerda){

				printf("going Right %i\n", fish->goingRight);
				if(fish->goingRight == 1){
					nextX += -left_coord[0];
					nextY += -left_coord[1];

					nextX += right_coord[0];
					nextY += right_coord[1];
				}
				else{
					nextX += -right_coord[0];
					nextY += -right_coord[1];

					nextX += left_coord[0];
					nextY += left_coord[1];
				}

				if(!is_valid_pos(nextX, nextY)){
					adjuste_pos_circle_scenery(&nextX, &nextY);
				}

				if(is_free_pos(nextX, nextY)){

					Object oldObj = fish->obj;
					fish->obj = move_object(fish->obj, nextX, nextY, fish->boardValue);
					
					if(distance(oldObj, fish->obj) == 0){
						//fish->predator = NULL;
						//fish->state = FISH_IDLE;
						move_to_random_pos(&fish->obj, m_mov_possib, fish->boardValue);
					}
					else{
						fish->orientation += -1;
					}
				}
				else{

					fish->predator = NULL;
					fish->state = FISH_IDLE;
				}				
			}

			create_fish_trail(fishOldObj, fish->obj);

			
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

		if( fish->trail == NULL)
			fish->state = FISH_IDLE;
		else{
			Trail* trail = fish->trail;
			int nextX = fish->obj.x;
			int nextY = fish->obj.y;

			if(fish->obj.x < trail->obj.x){
				nextX += 1;
			}
			else if(fish->obj.x > trail->obj.x){
				nextX += -1;
			}
			else{
				if(fish->obj.y < trail->obj.y){
					nextY += 1;
				}
				else if(fish->obj.y > trail->obj.y){
					nextY += -1;
				}
			}

			if(!is_valid_pos(nextX, nextY)){
				adjuste_pos_circle_scenery(&nextX, &nextY);
			}

			fish->obj = move_object(fish->obj, nextX, nextY, fish->boardValue);
			if(distance(fishOldObj, fish->obj) == 0){
				fish->trail = NULL;
				fish->state = FISH_IDLE;
			}
		}
	}

	if(fish->state == FISH_IDLE){

		move_to_random_pos(&fish->obj, m_mov_possib, fish->boardValue);
	}
}

int* get_right_based_on_dir(int dir){

	int *coord = (int*)malloc(2 * sizeof(int));
	dir = dir % 4;

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
	dir = dir % 4;

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

void create_fish_trail(Object oldObj, Object obj){

	//Se não tem rastro na posição antiga do tubarão - cria um novo rastro
	int diff_x = obj.x - oldObj.x;
	int diff_y = obj.y - oldObj.y;

	if(is_valid_pos(oldObj.x, oldObj.y)){
		
		Trail* trail = (Trail*)malloc(sizeof(Trail));
		trail->obj.x = oldObj.x;
		trail->obj.y = oldObj.y;
		trail->tempo = 0;
		trail->dir = getDir(diff_x, diff_y);
		trail->obj.sprite = m_trail_sprite;

		trail->obj = create_object(trail->obj, trail->obj.x, trail->obj.y, my_trail_value);

		insere_lista_final(m_li_trail, trail);

	}
	else if(has_trail_pos(oldObj.x, oldObj.y, 0)) {

		Trail* trail = get_trail_by_pos(oldObj.x, oldObj.y);

		if(trail != NULL){

			trail->tempo = 0;
			int oldDir = trail->dir;
			int currentDir = getDir(diff_x, diff_y);
			int newDir = (oldDir + currentDir) / 2;
			newDir = smoothDir(newDir);
			
			//Verificar se temos que espelhar, e caso positivo, espelhar de fato
			if(abs(currentDir - oldDir) >= 180){
				int espelhar = rand() % 2;
				if(espelhar == 1){
					if(newDir < 180){
						newDir += 180;
					}
					else{
						newDir += -180;
					}
				}
			}

			trail->dir = newDir;
		}
	}
}

void search_trail_to_follow(Fish* fish){

	int betterDistance = 1000;

	Elem* li = *m_li_trail;
	while(li != NULL){

		Trail* trail = getTrail(li);
		int d = distance(fish->obj, trail->obj);
		if(d <= fish->perceptionRange && d > 0){

			if(d < betterDistance){
				fish->trail = trail;
				betterDistance = d;
				fish->state = FISH_RUNNING;
			}
		}
		
		li = li->prox;
	}
}
