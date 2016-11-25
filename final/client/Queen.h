#pragma once

#include "parser.h"
#include "StepOffer.h"
#include <sstream>
#include <fstream>
#include "distcache.h"
#include "fleepath.h"

struct Queen : public MAP_OBJECT
{
	Queen(MAP_OBJECT& me, PARSER& parser_, DISTCACHE& mDistCache_, std::ofstream& mDebugLog_, FLEEPATH& mFleePath_) :
		MAP_OBJECT{me},
		parser{ parser_ },
		mDistCache{ mDistCache_ },
		mDebugLog{ mDebugLog_ },
		mFleePath{ mFleePath_ }
	{}


	PARSER& parser;
	DISTCACHE& mDistCache;
	std::ofstream& mDebugLog;
	FLEEPATH& mFleePath;
	// akit követek
	int enemyID = -1;

	void OfferOverrided()
	{
		enemyID = -1;
	}

	StepOffer CalcOffer()
	{
		StepOffer retval;
		const auto myCell = parser.GetAt(pos);
		
		//build
		if (mFleePath.GetDistToFriendlyCreep(pos) <= 1 && energy >= QUEEN_BUILD_CREEP_TUMOR_COST)
		{
			retval.Build.command.pos = pos;
			//minél több energia, minél kevesebb élet.
			retval.Build.certanty = (5 * energy) / QUEEN_MAX_ENERGY;
			retval.Build.certanty += (5 * (QUEEN_MAX_HP - hp)) / QUEEN_MAX_HP;
		}

		//attack
		if (enemyID >= 0)
		{
			MAP_OBJECT* enemy = nullptr;
			std::find(parser.Units.begin(), parser.Units.end(), [&](const MAP_OBJECT& unit){ return unit.id == enemyID; });

			if (enemy != nullptr)
			{
				auto enemyCell = parser.GetAt(enemy->pos);
				auto distance = mDistCache.GetDist(pos, enemy->pos);
				if (	(distance <= 1 && (myCell == eGroundType::CREEP || myCell == eGroundType::EMPTY))
					||	(distance < 5 && enemyCell == eGroundType::CREEP))
				{
					retval.Attack.command.c = eUnitCommand::CMD_ATTACK;
					retval.Attack.certanty = 10;
					retval.Attack.command.target_id = enemyID;
					return retval;
				}

			}

		}

		//move (defend)
		return retval;
	}

};