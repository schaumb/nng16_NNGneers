#include "stdafx.h"
#include "DumbAreaStrategy.h"
#include <algorithm>

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

	mState.Aggressivity = 1.0;
	mState.Attacked = 1.0;
	mState.Expanding = 1.0;
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
	for (const auto& q : mEnemyQueens)
	{
		if (mDistCache.GetDist(q.pos, mParser.OwnHatchery.pos) < 10)
		{
			mState.Attacked = 10.0;
			break;
		}
		switch (mParser.GetAt(q.pos))
		{
		case eGroundType::CREEP:
			mState.Attacked = std::min(mState.Attacked, 10.0);
			break;
		case eGroundType::CREEP_CANDIDATE_FRIENDLY:
			mState.Attacked = std::min(mState.Attacked, 5.0);
			break;
		case eGroundType::CREEP_CANDIDATE_BOTH:
			mState.Attacked = std::min(mState.Attacked, 8.0);
			break;
		default:
			break;
		}
	}
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
				if (minDist > dist && mParser.GetAt(currPos) == eGroundType::CREEP)
				{
					minDist = dist;
					BestPos = currPos;
				}
			}
		}
		if (BestPos.IsValid())
		{
			Step s;
			s.certanty = 1;
			switch (mParser.GetAt(BestPos))
			{
			case eGroundType::CREEP:
				s.certanty = 10;
				break;
			case eGroundType::CREEP_CANDIDATE_FRIENDLY:
				s.certanty = 8;
				break;
			case eGroundType::CREEP_CANDIDATE_BOTH:
				s.certanty = 5;
				break;
			case eGroundType::ENEMY_CREEP:
			case eGroundType::WALL:
				s.certanty = 0;
				break;
			default:
				break;
			}
			s.command.c = eUnitCommand::CMD_SPAWN;
			s.command.pos = BestPos;
			s.command.target_id = 0;
			if(s.certanty>0)
				mDesiredPositions.push_back(s);
		}
	}
	std::sort(mDesiredPositions.begin(), mDesiredPositions.end(), [](const Step& l, const Step& r) {return l.certanty > r.certanty; });
}

std::vector<Step> DumbAreaStrategy::GetStepOffers()
{
	return mDesiredPositions;
}
