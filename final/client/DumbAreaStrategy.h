#pragma once

#include "Client.h"
#include "IAreaStrategy.h"

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

	FuzzyState mState;
	
	DumbAreaStrategy(CLIENT& theClient)
		: mParser(theClient.mParser)
		, mDistCache(theClient.mDistCache)
		, mOwnTumors(), mEnemyTumors(), mEnemyQueens()
	{}
	void Update();

	/* INTERFACE */
	virtual void Process() override;
	virtual std::vector<Step> GetStepOffers() override;
};