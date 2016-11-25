#pragma once

#include "Client.h"
#include "IAreaStrategy.h"

struct FuzzyState
{
	float Aggressivity;
	float Expanding;
};

struct DumbAreaStrategy : public IAreaStrategy
{
	PARSER& mParser;
	DISTCACHE& mDistCache;
	std::vector<MAP_OBJECT> mOwnTumors;
	std::vector<MAP_OBJECT> mEnemyTumors;
	std::vector<MAP_OBJECT> mEnemyQueens;
	/*remove!!!*/
	std::vector<MAP_OBJECT> mOwnQueens;
	std::vector<POS> mDesiredPositions;
	
	DumbAreaStrategy(CLIENT& theClient)
		: mParser(theClient.mParser)
		, mDistCache(theClient.mDistCache)
		, mOwnTumors(), mEnemyTumors(), mEnemyQueens()
	{}
	virtual void Process() override;
	void Update();
};