#include <stdio.h>
#include <stdlib.h>
#include "Game.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

static const unsigned SCREEN_WIDTH = 480;
static const unsigned SCREEN_HEIGHT = 270;

int main(int argc, char *argv[])
{

    SDL_Window* window = NULL;
    bool isRunning = true;
    Game* game;
    

    SDL_Surface* screenSurface = NULL;

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
        
        if( window == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError());
            exit(-1);
        }
    }

    game = newGame(SCREEN_WIDTH,SCREEN_HEIGHT,NULL,NULL);

    GameInit(game);

    while(isRunning)
    {

    }


    SDL_DestroyWindow(window);
    SDL_Quit();
	
	return 0;
}