#ifndef GAME_H_
#define GAME_H_

#include "Matrix.h"
#include <SDL/SDL.h>

typedef struct 
{
	SparceArray field;
	unsigned time;
	unsigned round;	
	SDL_Surface* background;
	SDL_Surface* font;
	SDL_Rect resolution;
}Game;

Game* newGame(unsigned width, unsigned height, 
	         const SDL_Surface* _background, const SDL_Surface* font);




#endif