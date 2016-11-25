#pragma once

#include "parser.h"
#include "StepOffer.h"
#include <sstream>
#include <fstream>
#include "distcache.h"


struct Queen : public MAP_OBJECT
{
	Queen(PARSER& parser_, DISTCACHE& mDistCache_, std::ofstream& mDebugLog_) :
		parser{ parser_ },
		mDistCache{mDistCache_},
		mDebugLog{ mDebugLog_ }
	{}


	PARSER& parser;
	DISTCACHE& mDistCache;
	std::ofstream& mDebugLog;
	
	int OpponentID=-1;

	void OfferOverrided()
	{
		OpponentID = -1;
	}

	StepOffer CalcOffer()
	{
		StepOffer retval;
		const auto mycell = parser.GetAt(pos);
		if (OpponentID >= 0)
		{
			/*for (MAP_OBJECT& tumor : parser.CreepTumors)
			{
				if (tumor.side )
				{
				}
			}*/
			for (auto& enemy : parser.Units)
			{
				if (enemy.id == OpponentID)
				{
					if (mDistCache.GetDist(pos, enemy.pos) && (mycell == eGroundType::CREEP || mycell == eGroundType::EMPTY))
					{		
						retval.Attack.command.c = eUnitCommand::CMD_ATTACK;
						retval.Attack.certanty = 10;
						retval.Attack.command.
					}
				}
			}
		}

		return StepOffer();
	}
};