#ifndef AGENT_H_
#define AGENT_H_

#include <SDL.h>
#include "GameManager.h"
#include <math.h>
#include <float.h>

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
	int boardValue;
	int orientation;
	Shark* predator;
	Trail* trail;
} Fish;

struct shark {
	Object obj;
	StateShark state;
	int captureRange;
	int timeLapseToReproduction;
	int isLeader;
	int reproducedThisRound;
	int boardValue;
	Fish* prey;
	Trail* trail;
};

struct trail{
	Object obj;
	int dir;
	int tempo;
	int str;
};

int adjuste_pos_circle_scenery(int* x, int* y);
int is_valid_pos(int x, int y);
int is_free_pos(int x, int y);
Object move_object(Object obj, int newX, int newY, int value);
Object create_object(Object obj, int newX, int newY, int value);
void move_to_random_pos(Object* object, int mov_possib[4][2], int value);
int distance(Object a, Object b);
int isDead(Object obj);
int canReproduce(int currLife, int maxLife, int timeLapseToReproduction);
int isDistanceAcceptableToReproduce(int distance);

Object create_object_random_pos(int value);
Object create_object_father_pos(Object father1, Object father2, int value, int my_son_possib[8][2]);

#endif