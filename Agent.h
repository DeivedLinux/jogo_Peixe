#ifndef AGENT_H_
#define AGENT_H_

#include <SDL.h>

typedef enum 
{
	SHARK_IDLE,
	SHARK_CHASE,
	SHARK_ATTACK
} StateShark;

typedef enum
{
	FISH_IDLE,
	FISH_RUNNING
} StateFish;


typedef struct 
{
	int x;
	int y;
	int life;
	SDL_Surface* sprite;
} Object;

typedef struct{
	StateShark state;
	int captureRange;
	Object obj;
	int indexFish;
} Shark;

typedef struct{
	StateFish state;
	int perceptionRange;
	Object obj;
	int indexShark;
	int timeAfterReproduction;
} Fish;

typedef enum 
{
	FISH,
	SHARK
} TypeAgent;



//Agent* newAgent(TypeAgent type);

#endif