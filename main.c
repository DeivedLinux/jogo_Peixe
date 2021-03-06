#include <stdio.h>
#include <stdlib.h>
#include "Game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <complex.h>

static const unsigned SCREEN_WIDTH = 640;
static const unsigned SCREEN_HEIGHT = 640;

int main(int argc, char *argv[])
{

    SDL_Window* window = NULL;
    volatile bool isRunning = true;
    SDL_Surface* gScreen = NULL;
    Game* game = NULL;
    SDL_Surface* fishImg = NULL;
    SDL_Rect rect;
    volatile char v[10];
    int numberSharks = 10;
    int numberFish = 10;
   
    if(argc == 3)
    {
     	numberSharks = atoi(argv[1]);
     	numberFish = atoi(argv[2]);
    }

    if(SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(-1);
    }
    else
    {
        window = SDL_CreateWindow("SharkFatal", SDL_WINDOWPOS_UNDEFINED, 
                                 SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, 
                                 SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        
        if(window == NULL)
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError());
            exit(-1);
        }
        else
        {
            screenSurface = SDL_GetWindowSurface(window);
        }
    }
    IMG_Init(IMG_INIT_PNG);

    gScreen = SDL_LoadBMP("Fundo do Mar.bmp");

    game = newGame(SCREEN_WIDTH,SCREEN_HEIGHT,gScreen,NULL);

    GameInit(game,numberSharks,numberFish);

    while(isRunning)
    {
         game->play(game,&isRunning);
         SDL_UpdateWindowSurface(window);
         SDL_Delay(10);
    }
    
    destroyGame(game);
    SDL_FreeSurface(gScreen);
    SDL_DestroyWindow(window);
    SDL_Quit();
	
	return 0;
}