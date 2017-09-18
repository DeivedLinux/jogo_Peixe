#ifndef FISH_H_
#define FISH_H_

#include <SDL.h>
#include "Agent.h"
#include "GameManager.h"
#include "Lista.h"

void setupFishVariables(Lista* listaShark, Lista* listaFish, Lista* listaTrail, SDL_Surface* fishSprite[4], SDL_Surface* fishShinySprite[4],
					     SDL_Surface* trail_sprite, int movPossib[4][2], int  sonPossib[8][2], int fishValue, int trailValue);
	
Fish* create_fish();
Fish* crete_son_fish(Object father1, Object father2);
void fish_update();
void fish_movement(Fish* fish);	

void give_leader_to_fish(Fish* fish);
int has_fish_reproduced(Fish* fish);
int fish_reproduction(Elem* li, Fish* fish);
void search_predador(Fish* fish);
int* get_left_based_on_dir(int dir);
int* get_right_based_on_dir(int dir);
void create_fish_trail(Object oldObj, Object obj);
void search_trail_to_follow(Fish* fish);

#endif	