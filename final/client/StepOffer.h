#pragma once
#include "Step.h" 

struct StepOffer
{
	Step Build;
	Step Attack;
	int EnemysInHitRange = 0;
	StepOffer();
	~StepOffer();
};

