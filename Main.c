/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <SDL_image.h>
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
Board* create_board();
Settings* create_settings();

void fish_movement();
void shark_movement();

int main( int argc, char* args[] )
{	
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

		SDL_Delay( 100 );

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

	Elem *li = *li_shark;
	while(li != NULL){

		if(getShark(li)->state == SHARK_IDLE){

			int nextX;
			int nextY;
			do{
				int sorted = rand() % 4;
				nextX = getShark(li)->obj.x + mov_possib[sorted][0];
				nextY = getShark(li)->obj.y + mov_possib[sorted][1];
			} while(!isValidPos(nextX, nextY));

			getShark(li)->obj = moveObject(getShark(li)->obj, nextX, nextY);

		}

		li = li->prox;
	}
}

void fish_movement(){

	Elem *li = *li_fish;
	while(li != NULL){

		if(getFish(li)->state == FISH_IDLE){

			int nextX;
			int nextY;
			do{
				int sorted = rand() % 4;
				nextX = getFish(li)->obj.x + mov_possib[sorted][0];
				nextY = getFish(li)->obj.y + mov_possib[sorted][1];
			} while(!isValidPos(nextX, nextY));

			getFish(li)->obj = moveObject(getFish(li)->obj, nextX, nextY);

		}

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

Shark* create_shark(){
	Shark* shark;
	shark = (Shark*)malloc(sizeof(Shark));

	shark->obj = create_object_random_pos();
	shark->obj.life = settings->life; 
	shark->obj.sprite = shark_sprite;

	shark->state = SHARK_IDLE;
	shark->captureRange = settings->captureRange;
	shark->indexFish = -1; 

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
	fish->indexShark = -1;
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