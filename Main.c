/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include "Lista.h"
#include "Agent.h"
#include "GameManager.h"
#include "Shark.h"
#include "Fish.h"
#include "Trail.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 640;

const int mov_possib[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };
const int son_possib[8][2] = { {-1, -1}, {-1, 0}, {-1,  1}, {0,  1},
							   { 1,  1}, {1,  0}, { 1, -1}, {0, -1}};

//Valores únicos dos objetos do jogo - Pares não empedem a passagem - Impares impedem!
const int empty_value = 0;
const int fish_value = 1;
const int trail_value = 2;
const int shark_value = 3;

#define getShark(Obj) ((Shark*)Obj->dados)
#define getFish(Obj) ((Fish*)Obj->dados)
#define getTrail(Obj) ((Trail*)Obj->dados)

SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;

SDL_Surface* shark_sprite = NULL;
SDL_Surface* shark_shiny_sprite = NULL;
SDL_Surface* tile_sprite = NULL;
SDL_Surface* trail_sprite = NULL;

SDL_Surface* fish_sprite[4];
SDL_Surface* fish_shiny_sprite[4];

Lista* li_shark;
Lista* li_fish;

Lista* li_shark_trail;
Lista* li_fish_trail;

int i = 0, j = 0;


//SDL FUNCTIONS
void close();
int init();
int loadSprites();
SDL_Surface* loadSurface(char path[]);

//UPDATE FUNCIONS
void draw();
void update();


//GLOBAL FUNCIONS


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
	create_game(tile_sprite);

	//Inicializando listas
	li_shark = cria_lista();
	li_fish = cria_lista();
	li_shark_trail = cria_lista();
	li_fish_trail = cria_lista();
	setupSharkVariables(li_shark, li_fish, li_shark_trail, shark_sprite, shark_shiny_sprite, trail_sprite, &mov_possib, &son_possib, shark_value, trail_value);
	//setupFishVariables(li_shark, li_fish, li_fish_trail, fish_sprite, fish_shiny_sprite, trail_sprite, &mov_possib, &son_possib, fish_value, trail_value);
	
	//Inicializando função de random
	srand(time(NULL)); 
	
	
	for(i = 0; i < settings->amountSharks; i++){
		Shark* shark = create_shark(shark_value, shark_sprite);
		insere_lista_final(li_shark, shark);


		if(i == 0)
			give_leader_to_shark(shark);
	}

	for(i = 0; i < settings->amountFishes; i++){
		Fish* fish = create_fish();
		insere_lista_final(li_fish, fish);

		if(i == 0)
			give_leader_to_fish(fish);
	}

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
	fish_update();

	update_trails();
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
			SDL_BlitSurface(board->sprite, NULL, screenSurface, &rect );
		}
	}


	Elem* li;
	
	li = *li_shark_trail;
	if(settings->isTrailDrawable){
		while(li != NULL){
			rect.x = getTrail(li)->obj.x * rect.w;
			rect.y = getTrail(li)->obj.y * rect.h;

			SDL_BlitSurface(getTrail(li)->obj.sprite, NULL, screenSurface, &rect);
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

		Fish* fish = getFish(li);
		if(!fish->isLeader)
			SDL_BlitSurface(fish_sprite[fish->orientation], NULL, screenSurface, &rect);
		else
			SDL_BlitSurface(fish_shiny_sprite[fish->orientation], NULL, screenSurface, &rect);

		li = li->prox;
	}

	SDL_UpdateWindowSurface(window);
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
	SDL_FreeSurface( tile_sprite );
	SDL_FreeSurface( shark_sprite );
	tile_sprite = NULL;
	shark_sprite = NULL;
	//libera_lista(li_shark);
	//libera_lista(li_fish);

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

	tile_sprite = loadSurface("square.png");
	shark_sprite = loadSurface("shark.png");
	shark_shiny_sprite = loadSurface("shark_shiny.png");
	trail_sprite = loadSurface("trail.png");

	fish_sprite[0] = loadSurface("fish_0.png");
	fish_sprite[1] = loadSurface("fish_1.png");
	fish_sprite[2] = loadSurface("fish_2.png");
	fish_sprite[3] = loadSurface("fish_3.png");

	fish_shiny_sprite[0] = loadSurface("fish_shiny_0.png");
	fish_shiny_sprite[1] = loadSurface("fish_shiny_1.png");
	fish_shiny_sprite[2] = loadSurface("fish_shiny_2.png");
	fish_shiny_sprite[3] = loadSurface("fish_shiny_3.png");


	if(tile_sprite == NULL || fish_sprite == NULL || shark_sprite == NULL)
		return 0;

	return 1;
}
