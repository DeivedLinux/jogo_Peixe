/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "Agent.h"
#include "Lista.h"

#define BOARD_WIDTH 64
#define BOARD_HEIGHT 64

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 640;

#define getShark(Obj) ((Shark*)Obj->dados)
#define getFish(Obj) ((Fish*)Obj->dados)

typedef struct{
	int width;
	int height;
	SDL_Surface** sprite;
	int** spaces;
} Board;

typedef struct{
	int life;

	int amountSharks;
	int captureRange;
	
	int amountFishes;
	int perceptionRange;
	int timeLapseReproduction;
} Settings;

SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;

SDL_Surface* square_sprite = NULL;
SDL_Surface* fish_sprite = NULL;
SDL_Surface* shark_sprite = NULL;

Board* board;
Settings* settings;
Lista* li_shark;
Lista* li_fish;

int i = 0, j = 0;
const int mov_possib[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };
const int son_possib[8][2] = { {-1, -1}, {-1, 0}, {-1, 1}, {0, 1},
								{1, 1}, {1, 0}, {1, -1}, {0, -1}};


void close();
int init();
int loadSprites();
SDL_Surface* loadSurface(char path[]);

void draw();
void update();

int distance(Object a, Object b);
int isValidPos(int x, int y);
Object moveObject(Object obj, int newX, int newY);

Shark* create_shark();
Fish* create_fish();
Fish* create_son_fish(Object father1, Object father2);
Board* create_board();
Settings* create_settings();

void fish_movement();
void shark_movement();

int main( int argc, char* args[] ){	
	if(!init()){
		printf("Failed to initialize!\n");
		return 0;
	}

	if(!loadSprites()){
		printf("Failed to load sprites!\n");
		return 0;
	}

	//Inicializando função de random
	srand(time(NULL)); 

	//Inicializando variáveis do jogo
	createGame();

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

	//Main loop flag
	int quit = 0;

	//Event handler
	SDL_Event e;

	int nextStep = 1; //true for the frist draw

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

		update();
		draw();

		SDL_Delay(100);

	}

	close();

	return 0;
}

void update(){
	shark_movement();
	fish_movement();
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

	Elem* li = *li_shark;
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

void shark_movement(){

	Elem* li = *li_shark;
	while(li != NULL){

		if(getShark(li)->state == SHARK_IDLE){

			int nextX;
			int nextY;
			int tries = 0;
			do{
				int sorted = rand() % 4;
				nextX = getShark(li)->obj.x + mov_possib[sorted][0];
				nextY = getShark(li)->obj.y + mov_possib[sorted][1];
				tries++;
			} while(!isValidPos(nextX, nextY) || tries > 10);

			if(tries > 10){
				nextX = getShark(li)->obj.x;
				nextY = getShark(li)->obj.y;
			}

			getShark(li)->obj = moveObject(getShark(li)->obj, nextX, nextY);

			if(getShark(li)->prey == NULL){
				int betterDistance = 99;
				Elem* el_fish = *li_fish;
				while(el_fish != NULL){
					
					int d = distance(getShark(li)->obj, getFish(el_fish)->obj);
					if(d <= getShark(li)->captureRange && d < betterDistance){
						
						getShark(li)->prey = getFish(el_fish);
						getShark(li)->state = SHARK_CHASE;
						betterDistance = d;
					}

					el_fish = el_fish->prox;
				}
			}
		}
		else if(getShark(li)->state == SHARK_CHASE){

			Shark* shark = getShark(li);
			if(shark->prey == NULL){
				shark->state = SHARK_IDLE;
				continue;
			}

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

			shark->obj = moveObject(shark->obj, nextX, nextY);

			int d = distance(shark->obj, fish->obj);
			if(d <= 1){
				shark->state = SHARK_IDLE;
				remove_lista_fish(li_fish, fish->obj);
				shark->prey = NULL;
			}

		}

		li = li->prox;
	}
}


void fish_movement(){

	Elem *li = *li_fish;
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

			int nextX;
			int nextY;
			int tries = 0;
			do{
				int sorted = rand() % 4;
				nextX = fish->obj.x + mov_possib[sorted][0];
				nextY = fish->obj.y + mov_possib[sorted][1];
				tries++;
			} while(!isValidPos(nextX, nextY) || tries > 10);

			if(tries > 10){
				nextX = fish->obj.x;
				nextY = fish->obj.y;
			}

			fish->obj = moveObject(fish->obj, nextX, nextY);

			//Só reproduz no FISH_IDLE
			Elem* aux = li->prox;
			if(fish->timeLapseReproduction >= settings->timeLapseReproduction){
				while(aux != NULL){
					Fish* fish2 = getFish(aux);
					
					if(fish2->obj.x == fish->obj.x &&
						fish2->obj.y == fish->obj.y){
						aux = aux->prox;
						continue;
					}

					int d = distance(fish->obj, fish2->obj);
					if(d <= 2 && 
						fish2->timeLapseReproduction >= settings->timeLapseReproduction){

						fish->timeLapseReproduction = 0;
						fish2->timeLapseReproduction = 0;
						Fish* son = create_son_fish(fish->obj, fish2->obj);
						if(son != NULL)
							insere_lista_final(li_fish, son);

						break;
					}

					aux = aux->prox;
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

			fish->obj = moveObject(fish->obj, nextX, nextY);
		}

		fish->timeLapseReproduction++;
		li = li->prox;
	}
}

Object moveObject(Object obj, int newX, int newY){

	board->spaces[obj.x][obj.y] = 0;
	obj.x = newX;
	obj.y = newY;
	board->spaces[obj.x][obj.y] = 1;

	return obj;
}

int distance(Object a, Object b){

	return abs(a.x - b.x) + abs(a.y - b.y);
}

int isValidPos(int x, int y){

	if(x < 0 || x >= board->width || y < 0 || y >= board->height){
		return 0;
	}

	if(board->spaces[x][y] == 1){
		return 0;
	}

	return 1;
}

void createGame(){

	board = create_board();
	
	settings = create_settings();

	//FAZER SÓ OS TUBARÕES
	printf("Insira a quantidade de tubaroes: ");
	scanf("%i", &settings->amountSharks);

	printf("Insira a quantidade de peixes: ");
	scanf("%i", &settings->amountFishes);

	/*printf("Insira a range de captura dos tubaroes: ");
	scanf("%i", &settings->captureRange);

	printf("Insira a range de percepcao dos peixes: ");
	scanf("%i", &settings->perceptionRange);

	printf("Insira a quantidade passados de round para nova reproducao: ");
	scanf("%i", &settings->timeLapseReproduction);*/
	
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

	//default settings
	settings->life = 50;

	settings->amountSharks = 10;
	settings->captureRange = 10;

	settings->amountFishes = 10;
	settings->perceptionRange = 5;
	settings->timeLapseReproduction = 5;

	return settings;
}

Object create_object_random_pos(){
	Object obj;
	obj.x = -1;
	obj.y = -1;

	do{
		obj.x = rand() % board->width;
		obj.y = rand() % board->height; 
	}
	while(!isValidPos(obj.x, obj.y));

	obj = moveObject(obj, obj.x, obj.y);

	return obj;
}

Object create_object_father_pos(Object father1, Object father2){
	Object obj;
	obj.x = -1;
	obj.y = -1;
	int tries = 0;
	do{
		int sorted = rand() % 8;
		int fatherSorted = rand() % 2;
		obj.x = son_possib[sorted][0];
		obj.y = son_possib[sorted][1];
		if(fatherSorted == 1){
			obj.x += father1.x;
			obj.y += father1.y;
		}
		else{
			obj.x += father2.x;
			obj.y += father2.y;
		}

		tries++;

	}while(!isValidPos(obj.x, obj.y) || tries > 10);

	if(tries > 10){
		obj.x = -1;
		obj.y = -1;
		return obj;
	}

	obj = moveObject(obj, obj.x, obj.y);

	return obj;
}

Shark* create_shark(){
	Shark* shark;
	shark = (Shark*)malloc(sizeof(Shark));

	shark->obj = create_object_random_pos();
	shark->obj.life = settings->life; 
	shark->obj.sprite = shark_sprite;

	shark->state = SHARK_IDLE;
	shark->captureRange = settings->captureRange;
	shark->prey = NULL; 

	return shark;
}

Fish* create_fish(){
	Fish* fish;
	fish = (Fish*)malloc(sizeof(Fish));

	fish->obj = create_object_random_pos();
	fish->obj.life = settings->life;
	fish->obj.sprite = fish_sprite;

	fish->state = FISH_IDLE;
	fish->perceptionRange = settings->perceptionRange;
	fish->predator = NULL;
	fish->timeLapseReproduction = settings->timeLapseReproduction;

	return fish;
}

Fish* create_son_fish(Object father1, Object father2){
	Fish* fish;
	fish = (Fish*)malloc(sizeof(Fish));

	fish->obj = create_object_father_pos(father1, father2);
	if(fish->obj.x == -1 || fish->obj.y == -1)
		return NULL;

	fish->obj.life = settings->life;
	fish->obj.sprite = fish_sprite;

	fish->state = FISH_IDLE;
	fish->perceptionRange = settings->perceptionRange;
	fish->predator = NULL;
	fish->timeLapseReproduction = settings->timeLapseReproduction;

	return fish;
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
		window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
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
	fish_sprite = loadSurface("fish.png");
	shark_sprite = loadSurface("shark.png");

	if(square_sprite == NULL || fish_sprite == NULL || shark_sprite == NULL)
		return 0;

	return 1;
}