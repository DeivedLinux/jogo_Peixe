/*
 * ArrayList.c
 *
 *  Created on: 25/06/2017
 *      Author: Deived William
 */

#include "ArrayList.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static bool ListEmpty(List* list);
static void ListAdd(void* element, List **list);
static void ListDestroy(List **list);
static void* ListSearch(List *list,void* element, bool(*isEqual)(void*, void* ));
static unsigned getLength(List* list);

ArrayList* newArrayList()
{
	ArrayList* arrayList = NULL;

	arrayList = (ArrayList*)malloc(sizeof(ArrayList)*1000);
    arrayList->list = NULL;
	arrayList->isEmpty = ListEmpty;
	arrayList->addArray = ListAdd;
	arrayList->seach = ListSearch;
	arrayList->destroyList = ListDestroy;
	arrayList->size = getLength;

	return arrayList;
}

static bool ListEmpty(List *list)
{
   return list == NULL?1:0;
}
static void ListAdd(void* element, List** list)
{
	List *chainedList = (List*)malloc(sizeof(List));

	chainedList->information = element;
	chainedList->next = *list;
	*list = chainedList;
}
static void ListDestroy(List **list)
{
	List* listCpy = *list;

	while(listCpy != NULL)
	{
		/*guarda a referência da proximo objeto*/
		List* listTemp = ((List*)listCpy->next);
        free(listCpy);
        listCpy = listTemp;
        if(listTemp == NULL) *list = NULL;
	}

}
static void* ListSearch(List* list,void* element, bool(*isEqual)(void* , void*))
{
	List* temp;

	for(temp = list; temp != NULL; temp = temp->next)
	{
		if(isEqual(element, temp->information) == true)
		{
		  return temp->information;
		}
	}
    return NULL;
}
static unsigned getLength(List* list)
{
	unsigned i = 0;
	List* temp;

	for(temp = list; temp != NULL; temp = temp->next)
		i++;

	return i;
}