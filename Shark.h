#ifndef SHARK_H_
#define SHARK_H_

#include <SDL.h>
#include "Agent.h"
#include "GameManager.h"
#include "Lista.h"

Shark* create_shark(int shark_value, SDL_Surface** shark_sprite);
Shark* create_son_shark(Object father1, Object father2, int shark_value, int my_son_possib[8][2], SDL_Surface** shark_sprite);
void shark_update(Lista* li_shark, SDL_Surface** shark_shiny_sprite, int my_son_possib[8][2], int mov_possib[4][2], Lista* li_fish);
void shark_movement(Shark* shark, int mov_possib[4][2], Lista* li_fish);
int shark_reproduction(Elem* li, Shark* shark, int my_son_possib[8][2], Lista* li_shark);
void give_leader_to_shark(Shark* shark, SDL_Surface** shark_shiny_sprite);
int has_shark_reproduced(Shark* shark);
void search_fish(Shark* shark, Lista* li_fish);
int eat_fish_close(Shark* shark, Lista* li_fish);

#endif