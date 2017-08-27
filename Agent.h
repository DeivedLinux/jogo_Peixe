#ifndef AGENT_H_
#define AGENT_H_

#include <SDL2/SDL.h>


typedef enum 
{
	FISH,
	SHARK
}TypeAgent;

typedef struct 
{
	int x;
	int y;
	int life;
	TypeAgent type;
	SDL_Surface* sprite;
}Agent;

typedef enum 
{
	IDLE,
	CHASE,
	ATTACK
}StateShark;

Agent* newAgent(TypeAgent _type);

#endif