#include "stdafx.h"
#include "DumbAreaStrategy.h"

#define OWN 0
#define ENEMY 1

void DumbAreaStrategy::Update()
{
	if(mSpawnGoodness.empty())
		mSpawnGoodness.resize(mParser.w*mParser.h);
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
		{
			mOwnTumors.push_back(t);
			mSpawnGoodness[t.pos.y*mParser.w + t.pos.x] = -1;
		}
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
	int OwnCreep = 0;
	int EnemyCreep = 0;
	mDesiredTumorPositions.clear(); // for now
	mDesiredQueenPositions.clear();
	mAttackQueenPositions.clear();
	for (const auto& q : mEnemyQueens)
	{
		if (mDistCache.GetDist(q.pos, mParser.OwnHatchery.pos) < 10)
		{
			mState.Attacked = 10.0;
			Step s;
			s.certanty = 10;
			s.command.c = eUnitCommand::CMD_ATTACK;
			s.command.pos = q.pos;
			s.command.target_id = q.id;
			mAttackQueenPositions.push_back(s);
			break;
		}
		switch (mParser.GetAt(q.pos))
		{
		case eGroundType::CREEP:
		{
			mState.Attacked = std::min(mState.Attacked, 10.0);
			Step s;
			s.certanty = 10;
			s.command.c = eUnitCommand::CMD_ATTACK;
			s.command.pos = q.pos;
			s.command.target_id = q.id;
			mAttackQueenPositions.push_back(s);
		}	break;
		case eGroundType::CREEP_CANDIDATE_FRIENDLY:
		{
			mState.Attacked = std::min(mState.Attacked, 5.0);
			Step s;
			s.certanty = 5;
			s.command.c = eUnitCommand::CMD_ATTACK;
			s.command.pos = q.pos;
			s.command.target_id = q.id;
			mAttackQueenPositions.push_back(s);
		}	break;
		case eGroundType::CREEP_CANDIDATE_BOTH:
		{
			Step s;
			s.certanty = 8;
			s.command.c = eUnitCommand::CMD_ATTACK;
			s.command.pos = q.pos;
			s.command.target_id = q.id;
			mAttackQueenPositions.push_back(s);
			mState.Attacked = std::min(mState.Attacked, 8.0);
		}	break;
		default:
		{
			int dist = (unsigned short)mFleePath.GetDistToFriendlyCreep(q.pos);
			if (dist < 10)
			{
				int minDist = 10;
				POS BestPos(0, 0);
				for (auto p : mTumorCreepShape)
				{
					POS targetPos(q.pos.x + p.x, q.pos.y + p.y);
					if (targetPos.x < 0 || targetPos.y < 0 || mParser.GetAt(targetPos) != eGroundType::CREEP)
						continue;
					if (minDist > dist)
					{
						minDist = dist;
						BestPos = targetPos;
					}
				}
				if (BestPos.IsValid())
				{
					POS BestestPos(BestPos);
					int dx = std::min(1,std::max(-1,q.pos.x - BestPos.x));
					int dy = std::min(1,std::max(-1,q.pos.y - BestPos.y));
					if (mParser.GetAt(POS(BestPos.x + dx, BestestPos.y + dy)) == eGroundType::CREEP)
						BestestPos = POS(BestPos.x + dx, BestestPos.y + dy);
					else if (mParser.GetAt(POS(BestPos.x, BestestPos.y + dy)) == eGroundType::CREEP)
						BestestPos = POS(BestPos.x, BestestPos.y + dy);
					else if (mParser.GetAt(POS(BestPos.x + dx, BestestPos.y)) == eGroundType::CREEP)
						BestestPos = POS(BestPos.x + dx, BestestPos.y);
					Step s;
					s.certanty = 10-dist;
					s.command.c = eUnitCommand::CMD_MOVE;
					s.command.pos = BestestPos;
					s.command.target_id = q.id;
					mAttackQueenPositions.push_back(s);
					mState.Attacked = std::min(mState.Attacked, 3.0);
				}
			}
		}	break;
		}
	}
	for (const auto& t : mEnemyTumors)
	{
		int dist = (unsigned short)mFleePath.GetDistToFriendlyCreep(t.pos);
		if (dist <= 10)
		{
			int minDist = 10;
			POS BestPos(0, 0);
			for (auto p : mTumorCreepShape)
			{
				POS targetPos(t.pos.x + p.x, t.pos.y + p.y);
				if (targetPos.x < 0 || targetPos.y < 0 || mParser.GetAt(targetPos) != eGroundType::CREEP)
					continue;
				if (minDist > dist)
				{
					minDist = dist;
					BestPos = targetPos;
				}
			}
			if (BestPos.IsValid())
			{
				POS BestestPos(BestPos);
				int dx = std::min(1, std::max(-1, t.pos.x - BestPos.x));
				int dy = std::min(1, std::max(-1, t.pos.y - BestPos.y));
				if (mParser.GetAt(POS(BestPos.x + dx, BestestPos.y + dy)) == eGroundType::CREEP)
					BestestPos = POS(BestPos.x + dx, BestestPos.y + dy);
				else if (mParser.GetAt(POS(BestPos.x, BestestPos.y + dy)) == eGroundType::CREEP)
					BestestPos = POS(BestPos.x, BestestPos.y + dy);
				else if (mParser.GetAt(POS(BestPos.x + dx, BestestPos.y)) == eGroundType::CREEP)
					BestestPos = POS(BestPos.x + dx, BestestPos.y);
				Step s;
				s.certanty = 10-dist;
				s.command.c = eUnitCommand::CMD_MOVE;
				s.command.pos = BestestPos;
				s.command.target_id = t.id;
				mAttackQueenPositions.push_back(s);
				mState.Attacked = std::min(mState.Attacked, 2.0);
			}
		}
	}
	std::stable_sort(mAttackQueenPositions.begin(), mAttackQueenPositions.end(), [](const Step& l, const Step& r) {return l.certanty > r.certanty; });

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
			if (mSpawnGoodness[i*mParser.w + j] > -1)
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
					bool foundclose = false;
					for(auto& st: queenStep)
					{
						if (mDistCache.GetDist(st.command.pos, currPos) < 10)
						{
							if (st.certanty < std::max(1, area / 125))
							{
								foundclose = true;
								st.certanty = std::max(1, area / 125);
								st.command.pos = currPos;
								break;
							}
						}
					}
					if (!foundclose)
					{
						Step s;
						s.certanty = std::max(1, area / 125);
						s.command.c = eUnitCommand::CMD_SPAWN;
						s.command.pos = currPos;
						s.command.target_id = 0;
						queenStep.push_back(s);
					}
					if (mSpawnGoodness[i*mParser.w + j] > -1)
						mSpawnGoodness[i*mParser.w + j] = area;
				}
			}
		}
	}
	std::sort(queenStep.begin(), queenStep.end(), [](const Step& l, const Step& r) {return l.certanty > r.certanty; });
	int cmdbuffsize = std::min((int)queenStep.size(), 10);
	mDesiredQueenPositions.insert(mDesiredQueenPositions.begin(), queenStep.begin(), queenStep.begin() + cmdbuffsize);
}

std::vector<Step> DumbAreaStrategy::GetTumorSteps()
{
	return mDesiredTumorPositions;
}

std::vector<Step> DumbAreaStrategy::GetQueenSteps()
{
	return mDesiredQueenPositions;
}

std::vector<Step> DumbAreaStrategy::GetQueenAttacks()
{
	return mAttackQueenPositions;
}

FuzzyState DumbAreaStrategy::GetState()
{
	return mState;
}

int DumbAreaStrategy::GetSpawnGoodness(int x, int y)
{
	return mSpawnGoodness[mParser.w*y+x];
}
