#include "stdafx.h"
#include "DumbAreaStrategy.h"

#define OWN 0
#define ENEMY 1

void DumbAreaStrategy::Update()
{
	mOwnTumors.clear();
	mEnemyTumors.clear();
	mEnemyQueens.clear();
	/*remove*/
	mOwnQueens.clear();
	mOwnTumors.push_back(mParser.OwnHatchery);
	for (const auto& t : mParser.CreepTumors)
	{
		if (t.side == OWN)
			mOwnTumors.push_back(t);
		else
			mEnemyTumors.push_back(t);
	}
	for (const auto& u : mParser.Units)
	{
		if (u.side == ENEMY)
			mEnemyQueens.push_back(u);
		/*remove!*/
		else
			mOwnQueens.push_back(u);
	}

}

void DumbAreaStrategy::Process()
{
	Update();
	mDesiredPositions.clear(); // for now
	for (const auto& t : mOwnTumors)
	{
		int minDist = INT_MAX;
		POS BestPos(0,0);
		for (int i = 10; i-- > -9;)
		{
			for (int j = 9; j-- > -9;)
			{
				POS currPos(t.pos.x + j, t.pos.y + i);
				int dist = mDistCache.GetDist(currPos, mParser.EnemyHatchery.pos);
				if (minDist > dist)
				{
					minDist = dist;
					BestPos = currPos;
				}
			}
		}
		if (BestPos.IsValid())
			mDesiredPositions.push_back(BestPos);
	}
}