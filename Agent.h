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
typedef struct trail Trail;

typedef struct {
	Object obj;
	StateFish state;
	int perceptionRange;
	int timeLapseToReproduction;
	int isLeader;
	int reproducedThisRound;
	Shark* predator;
} Fish;

struct shark {
	Object obj;
	StateShark state;
	int captureRange;
	int timeLapseToReproduction;
	int isLeader;
	int reproducedThisRound;
	Fish* prey;
};

struct trail{
	Object obj;
	int dir;
	int tempo;
	int strength;
};

typedef enum 
{
	FISH,
	SHARK
} TypeAgent;

//Agent* newAgent(TypeAgent type);

#endif