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
    SDL_Surface* gScreen = NULL;
    Game* game = NULL;
    

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

    gScreen = SDL_LoadBMP("Fundo do Mar.bmp");
    game = newGame(SCREEN_WIDTH,SCREEN_HEIGHT,gScreen,NULL);

    GameInit(game);

    while(isRunning)
    {
         game->play(game);
         SDL_UpdateWindowSurface(window);
         SDL_Delay(10);
    }


    SDL_DestroyWindow(window);
    SDL_Quit();
	
	return 0;
}