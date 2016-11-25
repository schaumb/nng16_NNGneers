#pragma once

#include "parser.h"
#include "StepOffer.h"
#include <sstream>
#include <fstream>
#include "distcache.h"
#include "fleepath.h"
#include "Client.h"

struct Queen : public MAP_OBJECT
{
	Queen(MAP_OBJECT& me, CLIENT& client) :
		MAP_OBJECT{ me },
		parser{ client.mParser },
		mDistCache{ client.mDistCache },
		mDebugLog{ client.mDebugLog },
		mFleePath{ client.mFleePath }
	{}


	PARSER& parser;
	DISTCACHE& mDistCache;
	std::ofstream& mDebugLog;
	FLEEPATH& mFleePath;
	// akit követek
	int enemyID = -1;

	int Hits_to_die(MAP_OBJECT& unit)
	{
		auto cell = parser.GetAt(unit.pos);
		int HPChange = 0;
		if (cell == eGroundType::CREEP)
		{
			HPChange -= HP_DECAY_ON_ENEMY_CREEP;
		}
		if (cell == eGroundType::ENEMY_CREEP)
		{
			HPChange += HP_REGEN_ON_FRIENDLY_CREEP;
		}
		if (unit.side != 0)
		{
			HPChange += -1;
		}
		return std::ceil((double)unit.hp / (double)(QUEEN_DAMAGE - HPChange));
	}

	bool changeOpponent(MAP_OBJECT& current, MAP_OBJECT& other, int OtherDist)
	{
		auto distDiff = mDistCache.GetDist(pos, current.pos) - OtherDist * 2;
		return Hits_to_die(current) + distDiff > Hits_to_die(other);
	}

	inline bool canBuild()
	{//todo tumorra nem!peti fv
		return mFleePath.GetDistToFriendlyCreep(pos) <= 1 && energy >= QUEEN_BUILD_CREEP_TUMOR_COST;
	}
	StepOffer CalcOffer()
	{
		StepOffer retval;
		const auto myCell = parser.GetAt(pos);
		retval.Build.command.c = eUnitCommand::CMD_SPAWN;
		//build
		if (mFleePath.GetDistToFriendlyCreep(pos) <= 1 && energy >= QUEEN_BUILD_CREEP_TUMOR_COST)
		{
			retval.Build.command.pos = pos;
			//minél több energia, minél kevesebb élet.
			retval.Build.certanty = (5 * energy) / QUEEN_MAX_ENERGY;
			retval.Build.certanty += (5 * (QUEEN_MAX_HP - hp)) / QUEEN_MAX_HP;
		}

		//attack
		//in hits
		int ourArmyHP, enemyArmyHP;
		std::vector<MAP_OBJECT>::iterator opponent;
		for (auto unit = parser.Units.begin(); unit != parser.Units.end(); ++unit)
		{
			auto dist = mDistCache.GetDist(pos, unit->pos);
			if (dist < 5)
			{
				if (unit->side == 0)
				{
					ourArmyHP += Hits_to_die(*unit);
				}
				else
				{
					if (dist <= 1)
					{
						retval.EnemysInHitRange++;
					}
					enemyArmyHP += Hits_to_die(*unit);
				}
				if (changeOpponent(*opponent, *unit, dist))
				{
					opponent = unit;
				}
			}
		}
		int actualcreep = 0;

		//pánikszerûen építkezni kezdünk mert ölnek
		if (canBuild() && ceil((float)Hits_to_die(*this) / (retval.EnemysInHitRange)) <= floor((float)energy/QUEEN_BUILD_CREEP_TUMOR_COST) )
		{
			retval.Build.command.pos = pos;
			retval.Build.certanty = 10;
		}
		//ütik a hatcheryt, erõsebbek vagyunk, feláldozom magam(mert lesz másik)
		else if (opponent != parser.Units.end() && 
				(	mDistCache.GetDist(parser.OwnHatchery.pos, opponent->pos) < 3
				|| ourArmyHP >= enemyArmyHP
				||	parser.OwnHatchery.energy >= HATCHERY_BUILD_QUEEN_COST + (3 * actualcreep + 50) / 4 ))
		{	
			retval.Attack.command.c = eUnitCommand::CMD_ATTACK;
			retval.Attack.certanty = 10;
			retval.Attack.command.target_id = enemyID;
		}


		return retval;
	}
};
