#pragma once
#include "Step.h" 

struct StepOffer
{
	Step Build;
	Step Attack;
	Step Defend;
	StepOffer();
	~StepOffer();
};

