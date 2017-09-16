/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "Agent.h"
#include "Lista.h"

#define BOARD_WIDTH 64
#define BOARD_HEIGHT 64

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 640;

#define getShark(Obj) ((Shark*)Obj->dados)
#define getFish(Obj) ((Fish*)Obj->dados)
#define getTrail(Obj) ((Trail*)Obj->dados)

typedef struct{
	int width;
	int height;
	SDL_Surface** sprite;
	int** spaces;
} Board;

typedef struct{
	//Shark
	int amountSharks;
	int captureRange;
	int sharkLife;
	int sharkTimeLapseToReproduction;
	//Fish
	int amountFishes;
	int perceptionRange;
	int fishLife;
	int fishTimeLapseToReproduction;
	int isTrailDrawable;
} Settings;

SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;

SDL_Surface* square_sprite = NULL;
SDL_Surface* fish_sprite = NULL;
SDL_Surface* shark_sprite = NULL;
SDL_Surface* trail_sprite = NULL;

Board* board;
Settings* settings;
Lista* li_shark;
Lista* li_fish;
Lista* li_trail;

int i = 0, j = 0;
const int mov_possib[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };
const int son_possib[8][2] = { {-1, -1}, {-1, 0}, {-1,  1}, {0,  1},
							   { 1,  1}, {1,  0}, { 1, -1}, {0, -1}};

//Valores únicos dos objetos do jogo - Pares não empedem a passagem - Impares impedem!
const int empty_value = 0;
const int fish_value = 1;
const int trail_value = 2;
const int shark_value = 3;

void close();
int init();
int loadSprites();
SDL_Surface* loadSurface(char path[]);

void draw();
void update();

int distance(Object a, Object b);
int isValidPos(int x, int y);
Object moveObject(Object obj, int newX, int newY, int value);

Shark* create_shark();
Fish* create_fish();
void create_trail(Object oldObj, Object obj, int diff_x, int diff_y);
Fish* create_son_fish(Object father1, Object father2);
Shark* create_son_shark(Object father1, Object father2);
Board* create_board();
Settings* create_settings();
Trail* get_trail_by_pos(int x, int y);

void shark_update();
void shark_movement(Shark* shark);
void fish_update();
void fish_movement();

int* get_avaibles_mov_pos(Object obj, int* size);
int* get_avaibles_son_pos(Object obj, int* size);

int main( int argc, char* args[] ){	

	if(!init()){
		printf("Failed to initialize!\n");
		return 0;
	}

	if(!loadSprites()){
		printf("Failed to load sprites!\n");
		return 0;
	}

	//Inicializando variáveis do jogo
	createGame();

	//Inicializando função de random
	srand(time(NULL)); 
	

	li_shark = cria_lista();
	for(i = 0; i < settings->amountSharks; i++){
		Shark* shark = create_shark();
		insere_lista_final(li_shark, shark);
	}

	li_fish = cria_lista();
	for(i = 0; i < settings->amountFishes; i++){
		Fish* fish = create_fish();
		insere_lista_final(li_fish, fish);
	}

	li_trail = cria_lista();

	//Main loop flag
	int quit = 0;

	//Event handler
	SDL_Event e;

	int nextStep = 1; //true for the frist draw
	int firstTimeEver = 1; //para não fazer o primeiro update

	//While application is running
	while(!quit)
	{

		//Handle events on queue
		while( SDL_PollEvent( &e ) != 0 )
		{
			if(e.type == SDL_Quit)
			{
				quit = 1;
			}
			//User requests quit
			switch( e.type ){
					case SDL_KEYDOWN:
						switch( e.key.keysym.sym ){
		                    case SDLK_ESCAPE:
		                        quit = 1;
		                        break;
		                    case SDLK_RETURN:
		                    	nextStep = 1;
		                    	break;
	                }
				}     				
		}

		if(!nextStep)
			continue;

		nextStep = 0;

		///
		/*printf("\n");
		printf("\n");
		printf("\n");
		for(i = 0; i < board->width; i++){
			for(j = 0; j < board->height; j++){
				printf("%i", board->spaces[j][i]);
				//printf("%i", j);
			}
			printf("\n");
		}*/
		//

		if(firstTimeEver == 0){
			update();
		}
		else
			firstTimeEver = 0;

		draw();
		SDL_Delay(100);

	}

	close();

	return 0;
}

void update(){
	shark_update();

	fish_movement();
	update_trails();
}

int isDead(Object obj){

	if(obj.life <= 0)
		return 1;

	return 0;
}

int canReproduce(int currLife, int maxLife, int timeLapseToReproduction){

	if(maxLife - currLife > timeLapseToReproduction)
		return 1;

	return 0;
}

int hasSharkReproduced(Shark* shark){

	if(shark->reproducedThisRound == 1)
		return 1;

	return 0;
}

int isDistanceAcceptableToReproduce(int distance){

	if(distance <= 2)
		return 1;

	return 0;
}

int reproduceShark(Elem* li, Shark* shark){

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

Elem* removeElement(Lista* li, Elem* ele, Elem* ant){

	Elem* node = ele;

	if(ant == NULL){
		*li = node->prox;
	}
	else{
		ant->prox = node->prox;
	}

	ele = node->prox;
	free(node);

	return ele;
}

void shark_update(){

	Elem* li = *li_shark;
	Elem* ant = NULL; //Guardando o anterior para casos de remoção

	int count = 0;
	while(li != NULL){

		Shark* shark = getShark(li);

		if(isDead(shark->obj)){
			//printf("morte\n");
			li = removeElement(li_shark, li, ant);
		}
		else{ 
			if(hasSharkReproduced(shark)){
				//printf("já reproduziu %i\n", count);
				//Não faz nada - já procriou nesse round
			}
			else if(reproduceShark(li, shark)){
				//printf("reproduziu agora %i\n", count);
				//Não faz mais nada - acabou de reproduzir
			}
			else{
				//printf("anda %i\n", count);
				shark_movement(shark);		

			}
			shark->obj.life--;
				
			ant = li;
			li = li->prox;
		}
		count++;	
		shark->reproducedThisRound = 0;
	}
}

void draw(){

	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = SCREEN_WIDTH / board->width;
	rect.h = SCREEN_HEIGHT / board->height;


	for(i = 0; i < board->width; i++){
		for(j = 0; j < board->height; j++){
			rect.x = i * rect.w;
			rect.y = j * rect.h;
			SDL_BlitSurface( board->sprite, NULL, screenSurface, &rect );
		}
	}


	Elem* li;

	if(settings->isTrailDrawable){
		li = *li_trail;
		while(li != NULL){
			rect.x = getFish(li)->obj.x * rect.w;
			rect.y = getFish(li)->obj.y * rect.h;

			SDL_BlitSurface( getTrail(li)->obj.sprite, NULL, screenSurface, &rect);
			li = li->prox;
		}
	}

	li = *li_shark;
	while(li != NULL){
		rect.x = getShark(li)->obj.x * rect.w;
		rect.y = getShark(li)->obj.y * rect.h;

		SDL_BlitSurface( getShark(li)->obj.sprite, NULL, screenSurface, &rect);
		li = li->prox;
	}

	li = *li_fish;
	while(li != NULL){
		rect.x = getFish(li)->obj.x * rect.w;
		rect.y = getFish(li)->obj.y * rect.h;

		SDL_BlitSurface( getFish(li)->obj.sprite, NULL, screenSurface, &rect);
		li = li->prox;
	}

	SDL_UpdateWindowSurface(window);
}

int* get_dir_coordinates(int dir){

	int *coord = (int*)malloc(2 * sizeof(int));
	coord[0] = 0;
	coord[1] = -1;


	if(dir == 0){
		coord[0] = 0;
		coord[1] = -1;
	}
	if(dir == 90){
		coord[0] = 1;
		coord[1] = 0;
	}
	if(dir == 180){
		coord[0] = 0;
		coord[1] = 1;
	}
	if(dir == 270){
		coord[0] = -1;
		coord[1] = 0;
	}


	return coord;
}

int is_free_pos(int x, int y){

	if(board->spaces[x][y] % 2 == 0)
		return 1;

	return 0;
}

int adjuste_pos_circle_scenery(int* x, int* y){

	if(*x < 0){
		*x = board->width - 1;
	}
	else if(*x >= board->width){
		*x = 0;
	}
	else if(*y < 0){
		*y = board->height - 1;
	}
	else if(*y >= board->height){
		*y = 0;
	}
}


void move_to_random_pos(Object* object){

	
	int size = 0;
	int pos_avaible_to_move[4][2];

	for(i = 0; i < 4; i++){

		int posX = mov_possib[i][0] + object->x;
		int posY = mov_possib[i][1] + object->y;
		
		if(!isValidPos(posX, posY)){
			adjuste_pos_circle_scenery(&posX, &posY);
		}

		if(is_free_pos(posX, posY)){

			pos_avaible_to_move[size][0] = posX;
			pos_avaible_to_move[size][1] = posY;
			size++;
		}
	}

	if(size > 0){

		int sortedPos = rand() % size;
		*object = moveObject(*object, pos_avaible_to_move[sortedPos][0], pos_avaible_to_move[sortedPos][1], shark_value);
	}
}

void shark_movement(Shark* shark){

		//Primeiro checa se ele está em algum rastro
		/*if(board->spaces[shark->obj.x][shark->obj.y] == trail_value + shark_value){
			//Pegar rastro
			Trail* trail = get_trail_by_pos(shark->obj.x, shark->obj.y);
			int followTrail = rand() % 100 + 1;
			if(followTrail < trail->strength){
				//É pra seguir o rastro
				int* coordenates = get_dir_coordinates(trail->dir);
				int nextX = shark->obj.x + coordenates[0];
				int nextY = shark->obj.y + coordenates[1];

				Object oldObj = shark->obj;
				shark->obj = moveObject(shark->obj, nextX, nextY, shark_value);
				create_trail(oldObj, shark->obj, coordenates[0], coordenates[1]);
			}
			else{
				//É pra ir para qualquer umas das posições restantes
				int size = 0;
				int* poss_index = get_avaibles_mov_pos(shark->obj, &size);

				int sorted = poss_index[rand() % size];
				int nextX = shark->obj.x + mov_possib[sorted][0];
				int nextY = shark->obj.y + mov_possib[sorted][1];

				Object oldObj = shark->obj;
				shark->obj = moveObject(shark->obj, nextX, nextY, shark_value);
				create_trail(oldObj, shark->obj, mov_possib[sorted][0], mov_possib[sorted][1]);
			}

		}*/
		if(shark->state == SHARK_IDLE){

			/*int size = 0;
			int* poss_index = get_avaibles_mov_pos(shark->obj, &size);

			if(size != 0){

					int sorted = poss_index[rand() % size];
					int nextX = shark->obj.x + mov_possib[sorted][0];
					int nextY = shark->obj.y + mov_possib[sorted][1];

					Object oldObj = shark->obj;
					shark->obj = moveObject(shark->obj, nextX, nextY, shark_value);
					//create_trail(oldObj, shark->obj, mov_possib[sorted][0], mov_possib[sorted][1]);

				if(shark->prey == NULL){
					int betterDistance = 99;
					Elem* el_fish = *li_fish;
					while(el_fish != NULL){
						
						int d = distance(shark->obj, getFish(el_fish)->obj);
						if(d <= shark->captureRange && d < betterDistance){
							
							shark->prey = getFish(el_fish);
							shark->state = SHARK_CHASE;
							betterDistance = d;
						}

						el_fish = el_fish->prox;
					}
				}
			}*/
			move_to_random_pos(&shark->obj);
		}
		else if(shark->state == SHARK_CHASE){

			if(shark->prey == NULL){
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

				//shark->obj = moveObject(shark->obj, nextX, nextY, shark_value);
				Object oldObj = shark->obj;
				shark->obj = moveObject(shark->obj, nextX, nextY, shark_value);
				create_trail(oldObj, shark->obj, nextX - shark->obj.x, nextY - shark->obj.y);

				int d = distance(shark->obj, fish->obj);
				if(d <= 1){
					shark->state = SHARK_IDLE;
					remove_lista_fish(li_fish, fish->obj);
					shark->prey = NULL;
				}
			}

		}

}

void fish_movement(){

	/*Elem *li = *li_fish;
	while(li != NULL){

		Fish* fish = getFish(li);
		fish->state = FISH_IDLE;

		//Conferir se tem algum tubarão perto
		Elem* el_shark = *li_shark;
		int closerDistance = 9999;
		while(el_shark != NULL){
			Shark* shark = getShark(el_shark);

			int d = distance(shark->obj, fish->obj);
			if(d < fish->perceptionRange && d < closerDistance){
				closerDistance = d;
				fish->predator = shark;
				fish->state = FISH_RUNNING;
			}

			el_shark = el_shark->prox;
		}

		if(fish->state == FISH_IDLE){

			int size = 0;
			int* poss_index = get_avaibles_mov_pos(fish->obj, &size);

			if(size != 0){

				int sorted = poss_index[rand() % size];
				int nextX = fish->obj.x + mov_possib[sorted][0];
				int nextY = fish->obj.y + mov_possib[sorted][1];

				fish->obj = moveObject(fish->obj, nextX, nextY, fish_value);

				//Vai tentar ver se ele tem que reproduzir
				Elem* aux = li->prox;
				if(fish->timeLapseToReproduction >= settings->fishTimeLapseToReproduction){
					
					while(aux != NULL){
						Fish* fish2 = getFish(aux);

						int d = distance(fish->obj, fish2->obj);
						if(d <= 2 && 
							fish2->timeLapseToReproduction >= settings->fishTimeLapseToReproduction){

							fish->timeLapseToReproduction = 0;
							fish2->timeLapseToReproduction = 0;
							Fish* son = create_son_fish(fish->obj, fish2->obj);
							if(son != NULL)
								insere_lista_final(li_fish, son);

							break;
						}

						aux = aux->prox;
					}
				}
			}
		}
		else if(fish->state == FISH_RUNNING){

			Shark* shark = fish->predator;
			int nextX = fish->obj.x;
			int nextY = fish->obj.y;

			if(fish->obj.x >= shark->obj.x && fish->obj.x < board->width - 1){
				nextX += 1;
			}
			else if(fish->obj.x < shark->obj.x && fish->obj.x > 0){
				nextX += -1;
			}
			else{
				if(fish->obj.y >= shark->obj.y && fish->obj.y < board->height - 1){
					nextY += 1;
				}
				else if(fish->obj.y < shark->obj.y && fish->obj.y > 0){
					nextY += -1;
				}
			}

			fish->obj = moveObject(fish->obj, nextX, nextY, fish_value);
		}

		fish->timeLapseToReproduction++;
		li = li->prox;
	}*/
}

int decay_strength(int tempo){

	//printf("expoente %lf\n", -0.07 * tempo);
	//long double a = 1 * (pow(FLT_EPSILON, 0.07 * tempo));
	int a = 50 - pow(tempo, 2);
	return a;
}

void update_trails(){

	Elem* li = *li_trail;
	Elem* ant = NULL;
	while(li != NULL){

		Trail* trail = getTrail(li);
		trail->strength = decay_strength(trail->tempo);
		trail->tempo += 1;
		
		//remoção
		if(trail->tempo >= 7){

			board->spaces[trail->obj.x][trail->obj.y] -= trail_value;
			if(ant != NULL){
				ant->prox = li->prox;
			}
			else{
				*li_trail = li->prox;
			}

			Elem* no = li;
			li = li->prox;
			free(no);
		}
		else{
			ant = li;
			li = li->prox;
		}
	}
}

int* get_avaibles_mov_pos(Object obj, int* size){

	*size = 0;
	int pos_avaible[4] = { -1, -1, -1, -1};

	for(i = 0; i < 4; i++){

		int aux_x = obj.x + mov_possib[i][0];
		int aux_y = obj.y + mov_possib[i][1];

		//Se for uma posição valida no tabuleiro
		if(isValidPos(aux_x, aux_y)){
			//Checa se é uma posição com elementos "passaveis" (pares)
			if(board->spaces[aux_x][aux_y] % 2 == 0){
				pos_avaible[i] = i;
				(*size)++;
			}
		}
	}

	int* real_mov_pos = (int*)malloc((*size) * sizeof(int)); 
	int currentPos = 0;
	for(i = 0; i < 4; i++){
		if(pos_avaible[i] != -1){
			real_mov_pos[currentPos] = pos_avaible[i];
			currentPos++;
		}
	}

	return real_mov_pos;
}

int* get_avaibles_son_pos(Object obj, int* size){

	*size = 0;
	int pos_avaible[8] = { -1, -1, -1, -1, -1, -1, -1 , -1};

	for(i = 0; i < 8; i++){

		int aux_x = obj.x + son_possib[i][0];
		int aux_y = obj.y + son_possib[i][1];

		if(isValidPos(aux_x, aux_y)){
			(*size)++;
		}
	}

	int* real_mov_pos = (int*)malloc((*size) * sizeof(int)); 
	int currentPos = 0;
	for(i = 0; i < 8; i++){
		if(pos_avaible[i] != -1){
			real_mov_pos[currentPos] = pos_avaible[i];
			currentPos++;
		}
	}

	return real_mov_pos;

}

Object moveObject(Object obj, int newX, int newY, int value){

	if(isValidPos(obj.x, obj.y))
		board->spaces[obj.x][obj.y] += -value;
	
	obj.x = newX;
	obj.y = newY;
	board->spaces[obj.x][obj.y] += value;

	return obj;
}

int distance(Object a, Object b){

	return abs(a.x - b.x) + abs(a.y - b.y);
}

int isValidPos(int x, int y){

	if(x < 0 || x >= board->width || y < 0 || y >= board->height){
		return 0;
	}

	return 1;
}

int getDir(int x, int y){

	if(x == 0 && y == -1){
		return 0;
	}
	else if(x == 1 && y == 0){
		return 90;
	}
	else if(x == 0 && y == 1){
		return 180;
	}
	else if(x == -1 && y == 0){
		return 270;
	}

	return 0;
}

int smoothDir(int dir){

	int smoothedDir = 0;
	int betterProximity = 9999;
	int possibleDir[4] = {0, 90, 180, 270};

	//exceção se o angulo for > 360
	if(dir >= 360){
		return smoothedDir = 0;
	}	

	//para não arredondar sempre para um lado, as vezes rodamos o vetor de trás pra frente
	int trasPraFrente = rand() % 2;
	if(trasPraFrente == 0){
		for(i = 0; i < 4; i++){

			if(abs(dir - possibleDir[i]) < betterProximity){
				smoothedDir = possibleDir[i];
				betterProximity = abs(dir - possibleDir[i]);
			}
		}
	}
	else{
		for(i = 3; i >= 0; i--){

			if(abs(dir - possibleDir[i]) < betterProximity){
				smoothedDir = possibleDir[i];
				betterProximity = abs(dir - possibleDir[i]);
			}
		}
	}

	return smoothedDir;
}


void createGame(){

	board = create_board();	
	settings = create_settings();

	printf("Insira a quantidade de tubaroes: ");
	scanf("%i", &settings->amountSharks);

	printf("Insira a quantidade de peixes: ");
	scanf("%i", &settings->amountFishes);


	int wantExtraSettings = 0;
	printf("(1)=sim / (0)=nao. Mostrar opcoes extras? ");
	scanf("%i", &wantExtraSettings);

	if(wantExtraSettings){

		printf("Insira a range de captura dos tubaroes: ");
		scanf("%i", &settings->captureRange);

		printf("Insira a range de percepcao dos peixes: ");
		scanf("%i", &settings->perceptionRange);

		printf("Insira o tempo de amadurecimento para a reprodução dos tubaroes: ");
		scanf("%i", &settings->sharkTimeLapseToReproduction);

		printf("Insira o tempo de amadurecimento para a reprodução dos peixes: ");
		scanf("%i", &settings->fishTimeLapseToReproduction);

		printf("(1)=sim / (0)=nao. Mostrar rastros?");
		scanf("%i", &settings->isTrailDrawable);
	}
	
}

Board* create_board(){
	Board* board;
	board = (Board*)malloc(sizeof(Board));

	board->width = BOARD_WIDTH;
	board->height = BOARD_HEIGHT;
	board->sprite = square_sprite;

	board->spaces = (int**)malloc(board->width * sizeof(int*));
	for(i = 0; i < board->width; i++){
		board->spaces[i] = (int*)malloc(board->height * sizeof(int));
		for(j = 0; j < board->height; j++){
			board->spaces[i][j] = 0;
		}		
	}

	return board;
}

Settings* create_settings(){
	Settings* settings;
	settings = (Settings*)malloc(sizeof(Settings));

	//Default settings
	//Shark
	settings->amountSharks = 10;
	settings->captureRange = 10;
	settings->sharkLife = 50;
	settings->sharkTimeLapseToReproduction = 40;
	//Fish
	settings->amountFishes = 10;
	settings->perceptionRange = 5;
	settings->fishLife = 50;
	settings->fishTimeLapseToReproduction = 30;
	settings->isTrailDrawable = 1;

	return settings;
}

Object create_object_random_pos(int value){
	
	Object obj;
	obj.x = -1;
	obj.y = -1;

	int nextX = -1;
	int nextY = -1;

	do{
		nextX = rand() % board->width;
		nextY = rand() % board->height; 
	}
	while(!isValidPos(nextX, nextY) || !is_free_pos(nextX, nextY));


	obj = moveObject(obj, nextX, nextY, value);

	return obj;
}

Object create_object_father_pos(Object father1, Object father2, int value){
	
	Object obj;
	obj.x = -1;
	obj.y = -1;

	int size = 0;
	int pos_avaible_to_son[16][2];

	for(i = 0; i < 8; i++){

		int posX = son_possib[i][0] + father1.x;
		int posY = son_possib[i][1] + father1.y;
		
		if(!isValidPos(posX, posY)){
			adjuste_pos_circle_scenery(&posX, &posY);
		}

		if(is_free_pos(posX, posY)){

			pos_avaible_to_son[size][0] = posX;
			pos_avaible_to_son[size][1] = posY;
			size++;
		}
	}

	for(i = 0; i < 8; i++){

		int posX = son_possib[i][0] + father2.x;
		int posY = son_possib[i][1] + father2.y;
		
		if(!isValidPos(posX, posY)){
			adjuste_pos_circle_scenery(&posX, &posY);
		}

		if(is_free_pos(posX, posY)){

			pos_avaible_to_son[size][0] = posX;
			pos_avaible_to_son[size][1] = posY;
			size++;
		}
	}

	if(size > 0){

		int sortedPos = rand() % size;
		obj = moveObject(obj, pos_avaible_to_son[sortedPos][0], pos_avaible_to_son[sortedPos][1], shark_value);
	}


	return obj;
}

Shark* create_shark(){
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

	return shark;
}

Fish* create_fish(){
	Fish* fish;
	fish = (Fish*)malloc(sizeof(Fish));

	fish->obj = create_object_random_pos(fish_value);
	fish->obj.life = settings->fishLife;
	fish->obj.sprite = fish_sprite;

	fish->state = FISH_IDLE;
	fish->perceptionRange = settings->perceptionRange;
	fish->timeLapseToReproduction = settings->fishTimeLapseToReproduction;
	fish->isLeader = 0;
	fish->reproducedThisRound = 0;
	fish->predator = NULL;

	return fish;
}

Trail* get_trail_by_pos(int x, int y){

	Elem* li = *li_trail;
	while(li != NULL){
		Trail* trail = getTrail(li);

		if(trail->obj.x == x && trail->obj.y == y)
			return trail;

		li = li->prox;
	}

	return NULL;
}

void create_trail(Object oldObj, Object obj, int diff_x, int diff_y){
	
	if(distance(oldObj, obj) == 0){
		return;
	}

	//Se não tem rastro na posição antiga do tubarão - cria um novo rastro
	if(board->spaces[oldObj.x][oldObj.y] != trail_value && 
		board->spaces[oldObj.x][oldObj.y] != trail_value + fish_value && 
		board->spaces[oldObj.x][oldObj.y] != trail_value + shark_value){

		Trail* trail = (Trail*)malloc(sizeof(Trail));
		trail->obj.x = oldObj.x;
		trail->obj.y = oldObj.y;
		trail->tempo = 0;
		trail->dir = getDir(diff_x, diff_y);
		trail->obj.sprite = trail_sprite;

		trail->obj = moveObject(trail->obj, trail->obj.x, trail->obj.y, trail_value);

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

	//insere_lista_final(li_trail, trail);

}

Fish* create_son_fish(Object father1, Object father2){
	Fish* fish;
	fish = (Fish*)malloc(sizeof(Fish));

	fish->obj = create_object_father_pos(father1, father2, fish_value);
	if(fish->obj.x == -1 || fish->obj.y == -1)
		return NULL;

	fish->obj.life = settings->fishLife;
	fish->obj.sprite = fish_sprite;

	fish->state = FISH_IDLE;
	fish->perceptionRange = settings->perceptionRange;
	fish->timeLapseToReproduction = settings->fishTimeLapseToReproduction;
	fish->isLeader = 0;
	fish->predator = NULL;

	return fish;
}

Shark* create_son_shark(Object father1, Object father2){
	Shark* shark;
	shark = (Shark*)malloc(sizeof(Shark));

	shark->obj = create_object_father_pos(father1, father2, shark_value);
	shark->obj.life = settings->sharkLife; 
	shark->obj.sprite = shark_sprite;

	shark->state = SHARK_IDLE;
	shark->captureRange = settings->captureRange;
	shark->timeLapseToReproduction = settings->sharkTimeLapseToReproduction;
	shark->isLeader = 0;
	shark->reproducedThisRound = 0;
	shark->prey = NULL; 

	return shark;
}

SDL_Surface* loadSurface(char path[]){
    //The final optimized image
    SDL_Surface* optimizedSurface = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path );
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError() );
    }
    else
    {
        //Convert surface to screen format
        optimizedSurface = SDL_ConvertSurface( loadedSurface, screenSurface->format, NULL );
        if( optimizedSurface == NULL )
        {
            printf( "Unable to optimize image %s! SDeL Error: %s\n", path, SDL_GetError() );
        }

        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }

    return optimizedSurface;
}

void close(){
	//Free loaded image
	SDL_FreeSurface( square_sprite );
	SDL_FreeSurface( fish_sprite );
	SDL_FreeSurface( shark_sprite );
	square_sprite = NULL;
	fish_sprite = NULL;
	shark_sprite = NULL;
	libera_lista(li_shark);
	libera_lista(li_fish);

	//Destroy window
	SDL_DestroyWindow( window );
	window = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int init(){
	int sucess = 1;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		sucess = 0;
	}
	else
	{
		//Create window
		window = SDL_CreateWindow( "Jogo Peixe", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( window == NULL )
		{
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
			sucess = 0;
		}
	}

	//Get window surface
	screenSurface = SDL_GetWindowSurface( window );

	//Fill the surface white
	SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ) );

	SDL_UpdateWindowSurface( window );

	return sucess;
}

int loadSprites(){

	square_sprite = loadSurface("square.png");
	fish_sprite = loadSurface("fish3.png");
	shark_sprite = loadSurface("shark2.png");
	trail_sprite = loadSurface("trail.png");

	if(square_sprite == NULL || fish_sprite == NULL || shark_sprite == NULL)
		return 0;

	return 1;
}