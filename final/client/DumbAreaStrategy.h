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
	std::vector<Step> mDesiredTumorPositions;
	std::vector<Step> mDesiredQueenPositions;
	std::vector<int> mSpawnGoodness;

	std::vector<POS> mTumorCreepShape;

	FuzzyState mState;
	
	DumbAreaStrategy(CLIENT& theClient)
		: mParser(theClient.mParser)
		, mDistCache(theClient.mDistCache)
		, mOwnTumors(), mEnemyTumors(), mEnemyQueens()
	{
		const double limit = 9.5;
		for (int i = limit; i-- > -limit;)
			for (int j = limit; j-- > -limit;)
				if ((i*i + j*j) <= limit*limit)
					mTumorCreepShape.push_back(POS(j, i));
		std::sort(mTumorCreepShape.begin(), mTumorCreepShape.end(), [](const POS& l, const POS& r) { return (l.x + l.y) > (r.x + r.y); });
	}
	void Update();

	/* INTERFACE */
	virtual void Process() override;
	virtual std::vector<Step> GetTumorSteps() override;
	virtual std::vector<Step> GetQueenSteps() override;
	virtual FuzzyState GetState() override;
	virtual int GetSpawnGoodness(int x, int y) override;
};