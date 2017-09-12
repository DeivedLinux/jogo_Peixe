#include "Game.h"
#include <stdlib.h>
#include <stdio.h>
#include "Agent.h"
#include <stdbool.h>
#include <SDL2/SDL_image.h>
#include <math.h>

#define getGame(Obj)         ((Game*)(Obj))
#define getArrayList(Obj)    ((Game*)Obj)->field->matrix
#define castGameSparseArray(Obj)  ((Game*)Obj)->field
#define getAgent(Obj) ((Agent*)Obj->information)

#define FISH_WIDTH   10
#define FISH_HEIGHT  10

#define SHARK_WIDTH  10
#define SHARK_HEIGHT 10

#define Module(value) value >= 0?value:-value;

SDL_Surface* screenSurface = NULL;
static void PlayGame(void* game, bool* isRunning);
static bool isEqualCoord(void* el, void* info);
static void randPosition(SparseArray* sparse);
static SDL_Surface* imgFish = NULL;
static SDL_Surface* imgShark = NULL;



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
    unsigned orient;
    Agent agentSearch;

    for(temp = sparse->matrix->list; temp != NULL; temp = temp->next)
    {
        orient = rand()%4;

        switch(orient)
        {
            case NORTH:

                if(((Agent*)temp->information)->y < 640)
                {
                    agentSearch.x = ((Agent*)temp->information)->y;
                    ((Agent*)temp->information)->y += 3;
                }
                else ((Agent*)temp->information)->y = 0;
            break;

            case SOUTH:

                if(((Agent*)temp->information)->x >= 0)
                {
                    ((Agent*)temp->information)->x -= 3;
                }
                else ((Agent*)temp->information)->x = 639;

            break;

            case EAST:

                if(((Agent*)temp->information)->x < 640)
                {
                    ((Agent*)temp->information)->x += 3;
                }
                else ((Agent*)temp->information)->x = 0;

            break;

            case WEST:

                if(((Agent*)temp->information)->x >= 0)
                {
                    ((Agent*)temp->information)->x -= 3;
                }
                else ((Agent*)temp->information)->x = 639;
            break;

        }
    }

}

int distanceManhattan(Agent* Obj1, Agent* Obj2)
{
   // return Module(Obj1->x - Obj2->x) + Module(Obj1->y - Obj2->y);
     return (int)(abs((Obj1->x - Obj2->x)) + abs((Obj1->y - Obj2->y)));
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
void GameInit(Game* game, unsigned sharks, unsigned fishs)
{
	Agent* shark = NULL;
	unsigned i;
    Agent* fish = NULL;
    unsigned orient;

    
    imgFish = IMG_Load("fish.png");
    imgShark = IMG_Load("shark.png");

    shark = newAgent(SHARK);
    shark->blood= 3;
    shark->x = 10*(rand()%64);
    shark->y = 10*(rand()%64);
    shark->sprite = imgShark;
    shark->title = VETERAN;
    shark->time = 1;
    shark->orientation = NORTH;

    game->field->matrix->addArray(shark,&game->field->matrix->list);
    fish = newAgent(FISH);
    fish->x = 10*(rand()%64);
    fish->y = 10*(rand()%64);
    fish->sprite = imgFish;
    fish->title = VETERAN;
    fish->time = 1;
    fish->orientation = SOUTH;
    game->field->matrix->addArray(fish,&game->field->matrix->list);

    
    for(i = 0; i < fishs; i++)
    {
    	Agent* temp;

    	fish = newAgent(FISH);
    	fish->blood = 2;
        orient = rand()%4;
        fish->sprite = imgFish;
        fish->title = NOOB;

        do
        {
    		fish->x = 10*(rand()%64);
    		fish->y = 10*(rand()%64);
        
            switch(orient)
            {
                case NORTH:

                    fish->orientation = NORTH;
                break;

                case SOUTH:

                    fish->orientation = SOUTH;
                break;

                case WEST:

                    fish->orientation = WEST;
                break;

                case EAST:

                    fish->orientation = EAST;
                break;

                default:
                    break;
            }

    		temp = game->field->matrix->seach(game->field->matrix->list,fish,isEqualCoord);
        }while(temp != NULL);
        game->field->matrix->addArray(fish,&game->field->matrix->list);
    }

    for(i = 0; i < sharks; i++)
    {
        Agent* temp;
        int orient;

        shark = newAgent(SHARK);
        shark = newAgent(SHARK);
        shark->blood= 3;   
        shark->sprite = imgShark;
        shark->title = NOOB;
        shark->time = 1;
        orient = rand()%4;

        do
        {
            
            shark->x = 10*(rand()%64);
            shark->y = 10*(rand()%64);

            switch(orient)
            {
                case NORTH:

                    shark->orientation = NORTH;
                break;

                case SOUTH:

                    shark->orientation = SOUTH;
                break;

                case WEST:

                    shark->orientation = WEST;
                break;

                case EAST:

                    shark->orientation = EAST;
                break;

                default:
                    break;
            }
            temp = game->field->matrix->seach(game->field->matrix->list,shark,isEqualCoord);
        }while(temp != NULL);   
        game->field->matrix->addArray(shark,&game->field->matrix->list); 
    }
}
static void PlayGame(void* game, bool* isRunning)
{
	SDL_Rect sharkRect;
    SDL_Rect fishRect;
    unsigned i,j;
    Agent* tempAgent = NULL;
    Agent *agentSearch;
    SDL_Event event;
    bool nextStep = false;
    List* temp;
    Agent* noobFish;
    unsigned orient;

    while(SDL_PollEvent(&event) != 0 )
    {
        if(event.type == SDL_Quit)
        {
        
        }
        
        switch(event.type)
        {
            case SDL_KEYDOWN:
                
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:

                        *isRunning = false;
                    
                    break;
                    
                    case SDLK_RETURN:
                        
                        nextStep = true;
                        
                    break;
                }
            break;
        }     
    }
    SDL_BlitSurface(getGame(game)->background,NULL,screenSurface,NULL);  

    for(temp = getArrayList(game)->list; temp != NULL; temp = temp->next)
    {

        if(getAgent(temp)->type == FISH)
        {
            fishRect.x = getAgent(temp)->x;
            fishRect.y = getAgent(temp)->y;
            fishRect.w = FISH_WIDTH+getAgent(temp)->x;
            fishRect.h = FISH_HEIGHT+getAgent(temp)->y;   
            SDL_BlitSurface(getAgent(temp)->sprite,NULL,screenSurface,&fishRect); 
        }
        else if(getAgent(temp)->type == SHARK)
        {
            sharkRect.x = getAgent(temp)->x;
            sharkRect.y = getAgent(temp)->y;
            sharkRect.w = SHARK_WIDTH+getAgent(temp)->x;
            sharkRect.h = SHARK_HEIGHT+getAgent(temp)->y;
            SDL_BlitSurface(getAgent(temp)->sprite,NULL,screenSurface,&sharkRect); 
        }
        
        List* tempAgent = NULL;
        for(tempAgent = getArrayList(game)->list; tempAgent != NULL; tempAgent = tempAgent->next)
        {
            if((getAgent(tempAgent)->type == SHARK) && (getAgent(temp)->type == FISH))
            {
                if(distanceManhattan(getAgent(temp),getAgent(tempAgent)) >= -10 && distanceManhattan(getAgent(temp),getAgent(tempAgent)) <= 10)
                {
                  puts("comeu");
                }
            }
            else if((getAgent(tempAgent)->type == FISH) && (getAgent(temp)->type == FISH))
            {
                if(distanceManhattan(getAgent(temp),getAgent(tempAgent)) >= -5 && 
                    distanceManhattan(getAgent(temp),getAgent(tempAgent)) <= 5 && 
                    (getAgent(temp) != getAgent(tempAgent)))
                {
                    noobFish = newAgent(FISH);
                    noobFish->blood = 2;
                    orient = rand()%4;
                    noobFish->sprite = imgFish;
                    noobFish->title = NOOB;

                    do
                    {
                        noobFish->x = 10*(rand()%64);
                        noobFish->y = 10*(rand()%64);
        
                        switch(orient)
                        {
                            case NORTH:

                                noobFish->orientation = NORTH;
                            break;

                            case SOUTH:

                                noobFish->orientation = SOUTH;
                            break;

                            case WEST:

                                noobFish->orientation = WEST;
                            break;

                            case EAST:

                                noobFish->orientation = EAST;
                            break;

                            default:
                            
                                break;
                        }
                        agentSearch = getGame(game)->field->matrix->seach(getGame(game)->field->matrix->list,noobFish,isEqualCoord);
                    }while(agentSearch != NULL);
                    getGame(game)->field->matrix->addArray(noobFish,&getGame(game)->field->matrix->list);

                }
            }
            
        }
        
    }

    if(nextStep)
    {
        randPosition(castGameSparseArray(game));
        nextStep = false;
    }
     
}