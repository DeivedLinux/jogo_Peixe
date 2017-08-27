#ifndef MATRIX_H_
#define MATRIX_H_

#include "ArrayList.h"

typedef struct 
{
	ArrayList* list;
	struct 
	{
		int row;
		int column;
	}Dimension;
}SparseArray;

SparseArray* newSparceArray(int _row, int _column);



#endif