#pragma once

#include "Step.h"
#include <vector>

struct FuzzyState
{
	double Aggressivity;
	double Attacked;
	double Expanding;
};

struct IAreaStrategy
{
	virtual void Process() = 0;
	virtual std::vector<Step> GetStepOffers() = 0;
};
