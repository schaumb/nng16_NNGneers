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
		int maxarea = INT_MIN;
		POS BestPos(0,0);
		for (auto cp : mTumorCreepShape)
		{
			POS currPos(t.pos.x+cp.x, t.pos.y+cp.y);
			if (currPos.x < 0 || currPos.y < 0 || mParser.GetAt(currPos) != eGroundType::CREEP)
				continue;
			int area = 0;
			for (auto p : mTumorCreepShape)
			{
				POS destpos(currPos.x + p.x, currPos.y + p.y);
				if (destpos.x < 0 || destpos.y < 0)
					continue;
				eGroundType g = mParser.GetAt(destpos);
				if (g == eGroundType::EMPTY)
					area += 2;
				else if (g == eGroundType::CREEP_CANDIDATE_FRIENDLY || g == eGroundType::CREEP_CANDIDATE_BOTH)
					area += 1;
			}
			if (maxarea < area && area > 10)
			{
				maxarea = area;
				BestPos = currPos;
			}
		}
		if (BestPos.IsValid())
		{
			Step s;
			s.certanty = std::max(1,maxarea/25);
			s.command.c = eUnitCommand::CMD_SPAWN;
			s.command.pos = BestPos;
			s.command.target_id = t.id;
			if(s.certanty>0)
				mDesiredPositions.push_back(s);
		}
	}
	for (int i = 0; i < mParser.h; ++i)
	{
		for (int j = 0; j < mParser.w; ++j)
		{
			
		}
	}
	std::sort(mDesiredPositions.begin(), mDesiredPositions.end(), [](const Step& l, const Step& r) {return l.certanty > r.certanty; });
}

std::vector<Step> DumbAreaStrategy::GetStepOffers()
{
	return mDesiredPositions;
}
