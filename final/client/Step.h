#pragma once
#include "UnitCommand.h"
struct Step
{
	//min 1, max 10
	int certanty = 1;
	CMD command;
	Step();
	~Step();
};

