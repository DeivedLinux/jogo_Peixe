#ifndef TRAIL_H_
#define TRAIL_H_

#include <SDL.h>
#include "Agent.h"
#include "GameManager.h"
#include "Lista.h"
#include "Trail.h"

void setupTrail(Lista* listaTrail, int value);
int smoothDir(int dir);
int getDir(int x, int y);
Trail* get_trail_by_pos(int x, int y);
int has_trail_pos(int x, int y, int my_value);
int* get_dir_coordinates(int dir);
int decay_strength(int tempo);
void update_trails();


#endif