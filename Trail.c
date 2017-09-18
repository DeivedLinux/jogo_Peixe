#include "Trail.h"

#define getTrail(Obj) ((Trail*)Obj->dados)

Lista* li_trail;
int m_trail_value;


void setupTrail(Lista* listaTrail, int value){

	li_trail = listaTrail;
	m_trail_value = value;
}

int smoothDir(int dir){

	int smoothedDir = 0;
	int betterProximity = 9999;
	int possibleDir[4] = {0, 90, 180, 270};

	//exceção se o angulo for > 360
	if(dir >= 360){
		return smoothedDir = 0;
	}	

	//para não arredondar sempre para um lado, as vezes rodamos o vetor de trás pra frente
	int trasPraFrente = rand() % 2;
	if(trasPraFrente == 0){
		for(int i = 0; i < 4; i++){

			if(abs(dir - possibleDir[i]) < betterProximity){
				smoothedDir = possibleDir[i];
				betterProximity = abs(dir - possibleDir[i]);
			}
		}
	}
	else{
		for(int i = 3; i >= 0; i--){

			if(abs(dir - possibleDir[i]) < betterProximity){
				smoothedDir = possibleDir[i];
				betterProximity = abs(dir - possibleDir[i]);
			}
		}
	}

	return smoothedDir;
}

int getDir(int x, int y){

	if(x == 0 && y == -1){ //Norte
		return 0;
	}
	else if(x == 1 && y == -1){
		return 0;
	}
	else if(x == 1 && y == 0){ //Leste
		return 90;
	}
	else if(x == 1 && y == 1){
		return 90;
	}
	else if(x == 0 && y == 1){ //Sul
		return 180;
	}
	else if(x == -1 && y == 1){
		return 180;
	}
	else if(x == -1 && y == 0){ //Oeste
		return 270;
	}
	else if(x == -1 && y == -1){
		return 270;
	}


	return 0;
}

Trail* get_trail_by_pos(int x, int y){

	Elem* li = *li_trail;
	while(li != NULL){
		Trail* trail = getTrail(li);

		if(trail->obj.x == x && trail->obj.y == y)
			return trail;

		li = li->prox;
	}

	return NULL;
}

int has_trail_pos(int x, int y, int my_value){

	if(board->spaces[x][y] == m_trail_value + my_value)
		return 1;

	return 0;
}

int* get_dir_coordinates(int dir){

	int *coord = (int*)malloc(2 * sizeof(int));
	coord[0] = 0;
	coord[1] = -1;


	if(dir == 0){
		coord[0] = 0;
		coord[1] = -1;
	}
	if(dir == 90){
		coord[0] = 1;
		coord[1] = 0;
	}
	if(dir == 180){
		coord[0] = 0;
		coord[1] = 1;
	}
	if(dir == 270){
		coord[0] = -1;
		coord[1] = 0;
	}


	return coord;
}

int decay_strength(int tempo){

	//printf("expoente %lf\n", -0.07 * tempo);
	//long double a = 1 * (pow(FLT_EPSILON, 0.07 * tempo));
	int a = 81 - pow(tempo, 2);
	return a;
}

void update_trails(){

	Elem* li = *li_trail;
	Elem* ant = NULL;
	while(li != NULL){

		Trail* trail = getTrail(li);
		trail->str = decay_strength(trail->tempo);
		trail->tempo += 1;
		
		//remoção
		if(trail->tempo >= 9){

			board->spaces[trail->obj.x][trail->obj.y] -= m_trail_value;
			if(ant != NULL){
				ant->prox = li->prox;
			}
			else{
				*li_trail = li->prox;
			}

			Elem* no = li;
			li = li->prox;
			free(no);
		}
		else{
			ant = li;
			li = li->prox;
		}
	}
}