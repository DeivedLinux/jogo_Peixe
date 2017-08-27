#ifndef MATRIX_H_
#define MATRIX_H_

#include "ArrayList.h"

typedef struct 
{
	ArrayList* matrix;
	struct 
	{
		int row;
		int column;
	}Dimension;
}SparseArray;



#endif