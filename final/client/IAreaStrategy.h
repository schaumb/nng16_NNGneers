#pragma once

#include "Step.h"
#include <vector>

struct FuzzyState
{
	float Aggressivity;
	float Attacked;
	float Expanding;
};

struct IAreaStrategy
{
	virtual void Process() = 0;
	virtual std::vector<Step> GetStepOffers() = 0;
};