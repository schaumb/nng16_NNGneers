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
	mDesiredTumorPositions.clear(); // for now
	mDesiredQueenPositions.clear();
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
					area += 10;
				else if (g == eGroundType::CREEP_CANDIDATE_FRIENDLY || g == eGroundType::CREEP_CANDIDATE_BOTH)
					area += 5;
				else if (g == eGroundType::CREEP)
					area += 1;
			}
			if (maxarea < area /*&& area > 10*/)
			{
				maxarea = area;
				BestPos = currPos;
			}
		}
		if (BestPos.IsValid())
		{
			Step s;
			s.certanty = std::max(1,maxarea/125);
			s.command.c = eUnitCommand::CMD_SPAWN;
			s.command.pos = BestPos;
			s.command.target_id = t.id;
			if(s.certanty>0)
				mDesiredTumorPositions.push_back(s);
		}
	}
	std::sort(mDesiredTumorPositions.begin(), mDesiredTumorPositions.end(), [](const Step& l, const Step& r) {return l.certanty > r.certanty; });

	std::vector<Step> queenStep;
	for (int i = 0; i < mParser.h; ++i)
	{
		for (int j = 0; j < mParser.w; ++j)
		{
			mSpawnGoodness[i*mParser.w + j] = 0;
			int area = 0;
			POS currPos(j, i);
			if (mParser.GetAt(currPos) != eGroundType::CREEP)
			{
				continue;
			}
			int emptyCount = (mParser.GetAt(POS(currPos.x + 1, currPos.y)) == eGroundType::EMPTY ? 1 : 0) +
				(mParser.GetAt(POS(currPos.x - 1, currPos.y)) == eGroundType::EMPTY ? 1 : 0) +
				(mParser.GetAt(POS(currPos.x, currPos.y + 1)) == eGroundType::EMPTY ? 1 : 0) +
				(mParser.GetAt(POS(currPos.x, currPos.y - 1)) == eGroundType::EMPTY ? 1 : 0);
			if (emptyCount > 0)
			{
				for (auto cp : mTumorCreepShape)
				{
					POS destpos(currPos.x + cp.x, currPos.y + cp.y);
					if (destpos.x < 0 || destpos.y < 0)
						continue;
					eGroundType g = mParser.GetAt(destpos);
					if (g == eGroundType::EMPTY)
						area += 10;
					else if (g == eGroundType::CREEP_CANDIDATE_FRIENDLY || g == eGroundType::CREEP_CANDIDATE_BOTH)
						area += 5;
					else if(g == eGroundType::CREEP)
						area += 1;
				}
				if (area > 0)
				{
					Step s;
					s.certanty = std::max(1, area / 125);
					s.command.c = eUnitCommand::CMD_SPAWN;
					s.command.pos = currPos;
					s.command.target_id = 0;
					queenStep.push_back(s);
					mSpawnGoodness[i*mParser.w + j] = area;
				}
			}
		}
	}
	std::sort(queenStep.begin(), queenStep.end(), [](const Step& l, const Step& r) {return l.certanty > r.certanty; });
	mDesiredQueenPositions.insert(mDesiredQueenPositions.begin(), queenStep.begin(), queenStep.begin() + 10);
}

std::vector<Step> DumbAreaStrategy::GetTumorSteps()
{
	return mDesiredTumorPositions;
}

std::vector<Step> DumbAreaStrategy::GetQueenSteps()
{
	return mDesiredQueenPositions;
}

FuzzyState DumbAreaStrategy::GetState()
{
	return mState;
}