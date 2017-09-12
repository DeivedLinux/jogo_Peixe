#ifndef AGENT_H_
#define AGENT_H_

#include <SDL2/SDL.h>


typedef enum 
{
	FISH,
	SHARK
}TypeAgent;

typedef enum 
{
	NOOB,
	VETERAN	
}Title;


typedef enum 
{
	NORTH = 0,
	SOUTH,
	WEST,
	EAST
}Orientaion;

typedef struct 
{
	int x;
	int y;
	int blood;
	TypeAgent type;
	SDL_Surface* sprite;
	int time;
	Title title;
	Orientaion orientation;
}Agent;

typedef struct 
{
	int time;
	float angle;
}Trail;

typedef enum 
{
	IDLE,
	CHASE,
	ATTACK
}StateShark;

Agent* newAgent(TypeAgent _type);

#endif