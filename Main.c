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
#include "Lista.h"
#include "Agent.h"
#include "GameManager.h"
#include "Shark.h"

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
SDL_Surface* fish_sprite = NULL;
SDL_Surface* trail_sprite = NULL;

Lista* li_shark;
Lista* li_fish;
Lista* li_trail;

int i = 0, j = 0;


//SDL FUNCTIONS
void close();
int init();
int loadSprites();
SDL_Surface* loadSurface(char path[]);

//UPDATE FUNCIONS
void draw();
void update();


//FISH FUNCTIONS
Fish* create_son_fish(Object father1, Object father2);
void create_trail(Object oldObj, Object obj, int diff_x, int diff_y);
Trail* get_trail_by_pos(int x, int y);
void fish_update();
void fish_movement();

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

	//Inicializando função de random
	srand(time(NULL)); 
	
	li_shark = cria_lista();
	for(i = 0; i < settings->amountSharks; i++){
		Shark* shark = create_shark(shark_value, shark_sprite);
		insere_lista_final(li_shark, shark);


		if(i == 0)
			give_leader_to_shark(shark, shark_shiny_sprite);
	}

	li_fish = cria_lista();
	for(i = 0; i < settings->amountFishes; i++){
		Fish* fish = create_fish(fish_value, fish_sprite);
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
	shark_update(li_shark, shark_shiny_sprite, son_possib, mov_possib, li_fish);
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

	if(settings->isTrailDrawable){
		li = *li_trail;
		while(li != NULL){
			rect.x = getFish(li)->obj.x * rect.w;
			rect.y = getFish(li)->obj.y * rect.h;

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

				fish->obj = move_object(fish->obj, nextX, nextY, fish_value);

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

			fish->obj = move_object(fish->obj, nextX, nextY, fish_value);
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

	//insere_lista_final(li_trail, trail);

}

Fish* create_son_fish(Object father1, Object father2){
	Fish* fish;
	/*fish = (Fish*)malloc(sizeof(Fish));

	fish->obj = create_object_father_pos(father1, father2, fish_value);
	if(fish->obj.x == -1 || fish->obj.y == -1)
		return NULL;

	fish->obj.life = settings->fishLife;
	fish->obj.sprite = fish_sprite;

	fish->state = FISH_IDLE;
	fish->perceptionRange = settings->perceptionRange;
	fish->timeLapseToReproduction = settings->fishTimeLapseToReproduction;
	fish->isLeader = 0;
	fish->predator = NULL;*/

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
	SDL_FreeSurface( tile_sprite );
	SDL_FreeSurface( fish_sprite );
	SDL_FreeSurface( shark_sprite );
	tile_sprite = NULL;
	fish_sprite = NULL;
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
	fish_sprite = loadSurface("fish_3.png");
	shark_sprite = loadSurface("shark.png");
	shark_shiny_sprite = loadSurface("shark_shiny.png");
	trail_sprite = loadSurface("trail.png");

	if(tile_sprite == NULL || fish_sprite == NULL || shark_sprite == NULL)
		return 0;

	return 1;
}
