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

typedef struct shark Shark;

typedef struct{
	Object obj;
	StateFish state;
	int perceptionRange;
	Shark* predator;
	int timeLapseReproduction;
} Fish;

struct shark{
	Object obj;
	StateShark state;
	int captureRange;
	Fish* prey;
};

typedef enum 
{
	FISH,
	SHARK
} TypeAgent;



//Agent* newAgent(TypeAgent type);

#endif