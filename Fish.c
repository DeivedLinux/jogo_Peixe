#include "Fish.h"

Fish* create_fish(int fish_value, SDL_Surface** fish_sprite){

	Fish* fish;
	fish = (Fish*)malloc(sizeof(Fish));

	fish->obj = create_object_random_pos(fish_value);
	fish->obj.life = settings->fishLife;
	fish->obj.sprite = fish_sprite;

	fish->state = FISH_IDLE;
	fish->perceptionRange = settings->perceptionRange;
	fish->timeLapseToReproduction = settings->fishTimeLapseToReproduction;
	fish->isLeader = 0;
	fish->reproducedThisRound = 0;
	fish->predator = NULL;

	return fish;
}