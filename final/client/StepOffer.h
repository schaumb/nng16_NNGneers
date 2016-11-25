#pragma once
#include "Step.h" 

struct StepOffer
{
	Step Build;
	Step Attack;
	Step OnIdle;
	int EnemysInHitRange = 0;
	StepOffer();
	~StepOffer();
};

