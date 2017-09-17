#ifndef GAMEMANAGER_H_INCLUDED
#define GAMEMANAGER_H_INCLUDED

#include <SDL.h>
#include "Agent.h"

#define BOARD_WIDTH 64
#define BOARD_HEIGHT 64

typedef struct{
	int width;
	int height;
	SDL_Surface** sprite;
	int** spaces;
} Board;

typedef struct{
	//Shark
	int amountSharks;
	int captureRange;
	int sharkLife;
	int sharkTimeLapseToReproduction;
	//Fish
	int amountFishes;
	int perceptionRange;
	int fishLife;
	int fishTimeLapseToReproduction;
	int isTrailDrawable;
} Settings;

Board* board;
Settings* settings;

Board* create_board(SDL_Surface** tileSprite);
Settings* create_settings();
void create_game(SDL_Surface** tileSprite);

#endif