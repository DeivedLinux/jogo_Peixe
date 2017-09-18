#include "Agent.h"

int is_valid_pos(int x, int y){

	if(x < 0 || x >= board->width || y < 0 || y >= board->height){
		return 0;
	}

	return 1;
}

int is_free_pos(int x, int y){

	if(board->spaces[x][y] % 2 == 0)
		return 1;

	return 0;
}

int adjuste_pos_circle_scenery(int* x, int* y){

	if(*x < 0){
		*x = board->width - 1;
	}
	else if(*x >= board->width){
		*x = 0;
	}

	if(*y < 0){
		*y = board->height - 1;
	}
	else if(*y >= board->height){
		*y = 0;
	}
}

Object move_object(Object obj, int newX, int newY, int value){

	if(!is_free_pos(newX, newY))
		return obj;

	if(is_valid_pos(obj.x, obj.y))
		board->spaces[obj.x][obj.y] += -value;
	
	obj.x = newX;
	obj.y = newY;
	board->spaces[obj.x][obj.y] += value;

	return obj;
}

Object create_object(Object obj, int newX, int newY, int value){
	
	obj.x = newX;
	obj.y = newY;
	board->spaces[obj.x][obj.y] += value;
	return obj;
}

void move_to_random_pos(Object* object, int mov_possib[4][2], int value){

	int size = 0;
	int pos_avaible_to_move[4][2];

	for(int i = 0; i < 4; i++){

		int posX = mov_possib[i][0] + object->x;
		int posY = mov_possib[i][1] + object->y;
		
		if(!is_valid_pos(posX, posY)){
			adjuste_pos_circle_scenery(&posX, &posY);
		}

		if(is_free_pos(posX, posY)){

			pos_avaible_to_move[size][0] = posX;
			pos_avaible_to_move[size][1] = posY;
			size++;
		}
	}

	if(size > 0){

		int sortedPos = rand() % size;
		*object = move_object(*object, pos_avaible_to_move[sortedPos][0], pos_avaible_to_move[sortedPos][1], value);
	}
}

int distance(Object a, Object b){

	return abs(a.x - b.x) + abs(a.y - b.y);
}

int isDead(Object obj){

	if(obj.life <= 0)
		return 1;

	return 0;
}

int canReproduce(int currLife, int maxLife, int timeLapseToReproduction){

	if(maxLife - currLife > timeLapseToReproduction)
		return 1;

	return 0;
}

int isDistanceAcceptableToReproduce(int distance){

	if(distance <= 2)
		return 1;

	return 0;
}

Object create_object_random_pos(int value){
	
	Object obj;
	obj.x = -1;
	obj.y = -1;

	int nextX = -1;
	int nextY = -1;

	do{
		nextX = rand() % board->width;
		nextY = rand() % board->height; 
	}
	while(!is_valid_pos(nextX, nextY) || !is_free_pos(nextX, nextY));


	obj = move_object(obj, nextX, nextY, value);

	return obj;
}

Object create_object_father_pos(Object father1, Object father2, int shark_value, int my_son_possib[8][2]){
	
	Object obj;
	obj.x = -1;
	obj.y = -1;

	int size = 0;
	int pos_avaible_to_son[16][2];

	for(int i = 0; i < 8; i++){

		int posX = my_son_possib[i][0] + father1.x;
		int posY = my_son_possib[i][1] + father1.y;
		
		if(!is_valid_pos(posX, posY)){
			adjuste_pos_circle_scenery(&posX, &posY);
		}

		if(is_free_pos(posX, posY)){

			pos_avaible_to_son[size][0] = posX;
			pos_avaible_to_son[size][1] = posY;
			size++;
		}
	}

	for(int i = 0; i < 8; i++){

		int posX = my_son_possib[i][0] + father2.x;
		int posY = my_son_possib[i][1] + father2.y;
		
		if(!is_valid_pos(posX, posY)){
			adjuste_pos_circle_scenery(&posX, &posY);
		}

		if(is_free_pos(posX, posY)){

			pos_avaible_to_son[size][0] = posX;
			pos_avaible_to_son[size][1] = posY;
			size++;
		}
	}

	if(size > 0){

		int sortedPos = rand() % size;
		obj = move_object(obj, pos_avaible_to_son[sortedPos][0], pos_avaible_to_son[sortedPos][1], shark_value);
	}


	return obj;
}



