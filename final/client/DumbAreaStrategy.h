#pragma once

#include "Client.h"
#include "IAreaStrategy.h"
#include <algorithm>

struct DumbAreaStrategy : public IAreaStrategy
{
	PARSER& mParser;
	DISTCACHE& mDistCache;
	std::vector<MAP_OBJECT> mOwnTumors;
	std::vector<MAP_OBJECT> mEnemyTumors;
	std::vector<MAP_OBJECT> mEnemyQueens;
	/*remove!!!*/
	std::vector<MAP_OBJECT> mOwnQueens;
	std::vector<Step> mDesiredPositions;

	std::vector<POS> mTumorCreepShape;

	FuzzyState mState;
	
	DumbAreaStrategy(CLIENT& theClient)
		: mParser(theClient.mParser)
		, mDistCache(theClient.mDistCache)
		, mOwnTumors(), mEnemyTumors(), mEnemyQueens()
	{
		for (int i = 9; i-- > -9;)
			for (int j = 9; j-- > -9;)
				if ((i*i + j*j) <= 400)
					mTumorCreepShape.push_back(POS(j, i));
		std::sort(mTumorCreepShape.begin(), mTumorCreepShape.end(), [](const POS& l, const POS& r) { return (l.x + l.y) > (r.x + r.y); });
	}
	void Update();

	/* INTERFACE */
	virtual void Process() override;
	virtual std::vector<Step> GetStepOffers() override;
};