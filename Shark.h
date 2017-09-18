#ifndef SHARK_H_
#define SHARK_H_

#include <SDL.h>
#include "Agent.h"
#include "GameManager.h"
#include "Lista.h"
#include "Trail.h"
#include "Fish.h"

void setupSharkVariables(Lista* li_shark, Lista* li_fish, Lista* li_trail, SDL_Surface** shark_sprite, SDL_Surface** shark_shiny_sprite,
						 SDL_Surface** trail_sprite, int mov_possib[4][2], int my_son_possib[8][2], int shark_value, int trail_value);

Shark* create_shark();
Shark* create_son_shark(Object father1, Object father2);
void shark_update();
void shark_movement(Shark* shark);	
int shark_reproduction(Elem* li, Shark* shark);
void give_leader_to_shark(Shark* shark);
int has_shark_reproduced(Shark* shark);
void search_fish(Shark* shark);
int eat_fish_close(Shark* shark);
void search_trail(Shark* shark);

#endif