#include "Game.h"
#include <stdlib.h>
#include <stdio.h>
#include "Agent.h"
#include <stdbool.h>
#include <SDL2/SDL_image.h>

#define getGame(Obj)         ((Game*)(Obj))
#define getArrayList(Obj)    ((Game*)Obj)->field->matrix
#define castGameSparseArray(Obj)  ((Game*)Obj)->field

#define FISH_WIDTH   10
#define FISH_HEIGHT  10

#define SHARK_WIDTH  10
#define SHARK_HEIGHT 10

SDL_Surface* screenSurface = NULL;
static void PlayGame(void* game);

static bool isEqualCoord(void* el, void* info)
{
    if((((Agent*)el)->x == ((Agent*)info)->x) && ((Agent*)el)->y == ((Agent*)info)->y)
    {
        return true;
    }
    return false;
}

static void randPosition(SparseArray* sparse)
{
    List* temp = NULL;

    for(temp = sparse->matrix->list; temp != NULL; temp = temp->next)
    {
       ((Agent*)temp->information)->x = rand()%64;
       ((Agent*)temp->information)->y = rand()%64;
    }

}

void PrintMar(SparseArray* sparse)
{
    List* temp = NULL;

    for(temp = sparse->matrix->list; temp != NULL; temp = temp->next)
    {
        printf("%i %i\n",((Agent*)temp->information)->x, ((Agent*)temp->information)->y);
    }

}
void destroyGame(Game* game)
{
    destroySparseArray(game->field);
    if(game->background != NULL)
        SDL_FreeSurface(game->background);
    if(game->font != NULL)
        SDL_FreeSurface(game->font);
    free(game);
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
    SDL_Surface* imgFish = NULL;
    SDL_Surface* imgShark = NULL;

    
    imgFish = IMG_Load("fish.png");
    imgShark = IMG_Load("shark.png");

    shark = newAgent(SHARK);
    shark->blood= 3;
    shark->x = rand()%64;
    shark->y = rand()%64;
    shark->sprite = imgShark;
    
    game->field->matrix->addArray(shark,&game->field->matrix->list);

    
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
            fish->sprite = imgFish;

    		temp = game->field->matrix->seach(game->field->matrix->list,fish,isEqualCoord);
        }while(temp != NULL);
        game->field->matrix->addArray(fish,&game->field->matrix->list);
    }
}
static void PlayGame(void* game)
{
	SDL_Rect sharkRect;
    SDL_Rect fishRect;
    unsigned i,j;
    Agent* tempAgent = NULL;
    Agent agentSearch;


    
    SDL_BlitSurface(getGame(game)->background,NULL,screenSurface,NULL);  
    for(i = 0; i < 64; i++)
    {
        for(j = 0; j < 64; j++)
        {
            agentSearch.x = i;
            agentSearch.y = j;

            tempAgent = getArrayList(game)->seach(getArrayList(game)->list,&agentSearch,isEqualCoord);
            if(tempAgent != NULL && tempAgent->type == FISH)
            {
                fishRect.x = agentSearch.x;
                fishRect.y = agentSearch.y;
                fishRect.w = FISH_WIDTH+agentSearch.x;
                fishRect.h = FISH_HEIGHT+agentSearch.y;   
                SDL_BlitSurface(tempAgent->sprite,NULL,screenSurface,&fishRect); 
            }
            else if(tempAgent != NULL && tempAgent->type == SHARK)
            {
                sharkRect.x = agentSearch.x;
                sharkRect.y = agentSearch.y;
                sharkRect.w = SHARK_WIDTH+agentSearch.x;
                sharkRect.h = SHARK_HEIGHT+agentSearch.y;
                SDL_BlitSurface(tempAgent->sprite,NULL,screenSurface,&sharkRect); 
            }
        }
    }

    randPosition(castGameSparseArray(game));
    
}