#include "Matrix.h"
#include <stdlib.h>

SparseArray* newSparceArray(int _row, int _column)
{
	SparseArray* array;

	array = (SparseArray*)malloc(sizeof(SparseArray));

	array->Dimension.row = _row;
	array->Dimension.column = _column;
	array->matrix = newArrayList();

	return array;
}