#ifndef GAME_H_
#define GAME_H_

#include "Matrix.h"
#include <SDL2/SDL.h>

typedef struct 
{
	SparseArray* field;
	unsigned time;
	unsigned round;	
	const SDL_Surface* background;
	const SDL_Surface* font;
	SDL_Rect resolution;
	void(*play)(void*,bool*);
}Game;

 extern SDL_Surface* screenSurface;

Game* newGame(unsigned _width, unsigned _height, 
	         const SDL_Surface* _background, const SDL_Surface* _font);

void GameInit(Game* game, unsigned sharks, unsigned fishs);
void destroyGame(Game* game);
SDL_Surface* loadSurface(char path[]);


#endif