#include "Game.h"
#include <stdlib.h>
#include <stdio.h>

static void PlayGame(void* game);

Game* newGame(unsigned _width, unsigned _height, const SDL_Surface* _background, const SDL_Surface* _font)
{
	Game* game = NULL;

	game = (Game*)malloc(sizeof(Game));

	game->resolution.w = _width;
	game->resolution.h = _height;
	game->background = _background;
	game->font = _font;
	game->time = 0;
	game->round = 0;
	game->field = newSparceArray(64,64);
	game->play = PlayGame;
    
    return game;
}
static void PlayGame(void* game)
{
	



}