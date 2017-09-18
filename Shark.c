#include "Shark.h"
#include <math.h>

#define getFish(Obj) ((Fish*)Obj->dados)
#define getShark(Obj) ((Shark*)Obj->dados)
#define getTrail(Obj) ((Trail*)Obj->dados)

Lista* li_shark;
Lista* li_trail;
Lista* li_fish;

SDL_Surface** shark_shiny_sprite;
SDL_Surface** shark_sprite;
SDL_Surface** trail_sprite;

int my_mov_possib[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };
int my_son_possib[8][2] = { {-1, -1}, {-1, 0}, {-1,  1}, {0,  1},
							{ 1,  1}, {1,  0}, { 1, -1}, {0, -1}};
int m_shark_value = 0;
int m_trail_value = 0;


void setupSharkVariables(Lista* listaShark, Lista* listaFish, Lista* listaTrail, SDL_Surface** sharkSprite, SDL_Surface** sharkShinySprite,
					     SDL_Surface** trail_sprite, int movPossib[4][2], int  sonPossib[8][2], int sharkValue, int trailValue){
	
	li_shark = listaShark;
	li_fish = listaFish;
	li_trail = listaTrail;

	shark_sprite = sharkSprite;
	shark_shiny_sprite = sharkShinySprite;

	m_shark_value = sharkValue;
	m_trail_value = trailValue;

	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 2; j ++){
			my_mov_possib[i][j] = movPossib[i][j];
		}
	}

	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 2; j ++){
			my_son_possib[i][j] = sonPossib[i][j];
		}
	}

	setupTrail(listaTrail, m_trail_value);
}

Shark* create_shark(){
	Shark* shark;
	shark = (Shark*)malloc(sizeof(Shark));

	shark->obj = create_object_random_pos(m_shark_value);
	shark->obj.life = settings->sharkLife; 
	shark->obj.sprite = shark_sprite;

	shark->state = SHARK_IDLE;
	shark->captureRange = settings->captureRange;
	shark->timeLapseToReproduction = settings->sharkTimeLapseToReproduction;
	shark->isLeader = 0;
	shark->reproducedThisRound = 0;
	shark->prey = NULL; 
	shark->trail = NULL;
	shark->boardValue = m_shark_value;

	return shark;
}

Shark* create_son_shark(Object father1, Object father2){
	Shark* shark;
	shark = (Shark*)malloc(sizeof(Shark));

	shark->obj = create_object_father_pos(father1, father2, m_shark_value, my_son_possib);
	shark->obj.life = settings->sharkLife; 
	shark->obj.sprite = shark_sprite;

	shark->state = SHARK_IDLE;
	shark->captureRange = settings->captureRange;
	shark->timeLapseToReproduction = settings->sharkTimeLapseToReproduction;
	shark->isLeader = 0;
	shark->reproducedThisRound = 0;
	shark->prey = NULL; 
	shark->trail = NULL;
	shark->boardValue = m_shark_value;

	return shark;
}

void shark_update(){
	
	Elem* li = (*li_shark);
	Elem* ant = NULL; //Guardando o anterior para casos de remoção
	
	while(li != NULL){

		Shark* shark = getShark(li);

		if(isDead(shark->obj)){

			if(shark->isLeader && li->prox != NULL)
				give_leader_to_shark(getShark(li->prox));

			li = removeElement(li_shark, li, ant);
		}
		else{ 
			if(has_shark_reproduced(shark)){
				//Já reproduziu nessa rodada - Não faz mais nada
				//printf("ja reproduziu\n");
			}
			else if(shark_reproduction(li, shark)){
				//Conseguiu reproduzir  - Não faz mais nada
				//printf("reproduziu agora!\n");
			}
			else{
				shark_movement(shark);		
			}
			shark->obj.life--;
				
			ant = li;
			li = li->prox;
		}
		shark->reproducedThisRound = 0;
	}
}

void shark_movement(Shark* shark){

	Object sharkOldObj = shark->obj;
	if(eat_fish_close(shark)){

		if(shark->isLeader){
			create_shark_trail(sharkOldObj, shark->obj);
		}

		return;
	}


	if(shark->isLeader){

		if(shark->prey == NULL)
			search_fish(shark);
	}
	else{

		if(has_trail_pos(shark->obj.x, shark->obj.y, m_shark_value)){
			
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
		}
	}


	
	if(shark->state == SHARK_CHASE && shark->isLeader){

		if( shark->prey == NULL)
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


			//criar trail aqui
			Object oldObj = shark->obj;
			shark->obj = move_object(shark->obj, nextX, nextY, shark->boardValue);
			create_shark_trail(sharkOldObj, shark->obj);
			if(distance(oldObj, shark->obj) == 0){
				shark->prey = NULL;
				shark->state = SHARK_IDLE;
			}
		}
	}
	else if(shark->state == SHARK_CHASE){

		if( shark->trail == NULL)
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
		}

	}
	
	if(shark->state == SHARK_IDLE){

		move_to_random_pos(&shark->obj, my_mov_possib, shark->boardValue);
	}

}

int shark_reproduction(Elem* li, Shark* shark){

	if(canReproduce(shark->obj.life, settings->sharkLife, shark->timeLapseToReproduction)){

		Elem* aux = li->prox;
		while(aux != NULL){

			Shark* shark2 = getShark(aux);

			int d = distance(shark->obj, shark2->obj);
			if(isDistanceAcceptableToReproduce(d) && canReproduce(shark2->obj.life, settings->sharkLife, shark2->timeLapseToReproduction)){

				//shark->timeLapseToReproduction += 10;
				//shark2->timeLapseToReproduction += 10;
				Shark* son = create_son_shark(shark->obj, shark2->obj);
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

void give_leader_to_shark(Shark* shark){

	shark->isLeader = 1;
	shark->obj.sprite = shark_shiny_sprite;
}

int has_shark_reproduced(Shark* shark){

	if(shark->reproducedThisRound == 1)
		return 1;

	return 0;
}

void search_fish(Shark* shark){

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

void search_trail(Shark* shark){

	int betterDistance = 1000;

	Elem* li = *li_trail;
	while(li != NULL){

		Trail* trail = getTrail(li);
		int d = distance(shark->obj, trail->obj);
		if(d <= shark->captureRange && d > 0){

			if(d < betterDistance){
				shark->trail = trail;
				betterDistance = d;
				shark->state = SHARK_CHASE;
			}
		}
		
		li = li->prox;
	}
}

int eat_fish_close(Shark* shark){

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
				shark->prey = NULL;
				shark->obj = move_object(shark->obj, fish->obj.x, fish->obj.y, m_shark_value);
				board->spaces[shark->obj.x][shark->obj.y] = m_shark_value;
				
				if(li->prox != NULL && fish->isLeader){
					Fish* fish2 = getFish(li->prox);
					if(fish2 != NULL){
						fish2->isLeader = 1;
					}
				}
				removeElement(li_fish, li, ant);
				return 1;
			}
		}
		
		ant = li;
		li = li->prox;
	}

	return 0;
}


void create_shark_trail(Object oldObj, Object obj){

	//Se não tem rastro na posição antiga do tubarão - cria um novo rastro
	int diff_x = obj.x - oldObj.x;
	int diff_y = obj.y - oldObj.y;

	if(is_valid_pos(oldObj.x, oldObj.y)){
		
		Trail* trail = (Trail*)malloc(sizeof(Trail));
		trail->obj.x = oldObj.x;
		trail->obj.y = oldObj.y;
		trail->tempo = 0;
		trail->dir = getDir(diff_x, diff_y);
		trail->obj.sprite = trail_sprite;

		trail->obj = create_object(trail->obj, trail->obj.x, trail->obj.y, m_trail_value);

		insere_lista_final(li_trail, trail);

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

	/*if(board->spaces[oldObj.x][oldObj.y] != m_trail_value && 
		board->spaces[oldObj.x][oldObj.y] != m_trail_value + fish_value && 
		board->spaces[oldObj.x][oldObj.y] != m_trail_value + shark_value){

		

		trail->obj = move_object(trail->obj, trail->obj.x, trail->obj.y, trail_value);

		insere_lista_final(li_trail, trail);
	}
	else{
		//Tem rastro nessa posição
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

	/*Trail* trail = (Trail*)malloc(sizeof(Trail));
	trail->obj.x = obj.x;
	trail->obj.y = obj.y;
	trail->tempo = 10;
	trail->dir = 1;
	trail->obj.sprite = trail_sprite;*/

	//insere_lista_final(li_trail, trail);*/

}

