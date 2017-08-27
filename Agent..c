#include "Agent.h"
#include <stdlib.h>
#include <stdio.h>


Agent* newAgent(TypeAgent _type)
{
	Agent* agent = NULL;

	agent = (Agent*)malloc(sizeof(Agent));

	agent->type = _type;
	agent->x = 0;
	agent->y = 0;
	agent->life = 0;

	return agent;
}