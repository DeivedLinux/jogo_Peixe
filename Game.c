#include "Game.h"
#include <stdlib.h>
#include <stdio.h>
#include "Agent.h"
#include <stdbool.h>

#define getGame(Obj)    ((Game*)(Obj))
#define getList(Obj)    field->list

static void PlayGame(void* game);
static bool isEqualCoord(void* el, void* info)
{
	return ((Agent*)el)->x == ((Agent*)info)->x && ((Agent*)el)->y == ((Agent*)info)->y?true:false;
}

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
void GameInit(Game* game)
{
	Agent* shark = NULL;
	unsigned i;

    shark = newAgent(SHARK);
    shark->blood= 3;
    shark->x = rand()%64;
    shark->y = rand()%64;
    
    game->field->list->addArray(shark,&game->field->list->list);

    for(i = 0; i < 10; i++)
    {
    	Agent* fish = NULL;
    	Agent* temp;

    	fish = newAgent(FISH);
    	fish->blood = 2;

        do
        {
    		fish->x = rand()%64;
    		fish->y = rand()%64;
    		temp = game->field->list->seach(game->field->list->list,fish,isEqualCoord);
        }while(temp != NULL);
        game->field->list->addArray(fish,&game->field->list->list);
    }
}
static void PlayGame(void* game)
{
	



}